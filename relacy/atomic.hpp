/*  Relacy Race Detector
 *  Copyright (c) 2008-2013, Dmitry S. Vyukov
 *  All rights reserved.
 *  This software is provided AS-IS with no warranty, either express or implied.
 *  This software is distributed under a license and may not be copied,
 *  modified or distributed except as expressly authorized under the
 *  terms of the license contained in the file LICENSE in this distribution.
 */

#ifndef RL_ATOMIC_HPP
#define RL_ATOMIC_HPP
#ifdef _MSC_VER
#   pragma once
#endif



#include "base.hpp"
#include "context.hpp"
#include "memory_order.hpp"
#include "signature.hpp"
#include "atomic_events.hpp"
#include "waitset.hpp"
#include "rmw.hpp"


namespace rl
{


template<typename T>
class atomic;


template<bool> struct bool_t {};



template<typename T>
class atomic_proxy_const
{
public:
    atomic_proxy_const(atomic<T> const /*volatile*/& var, debug_info_param info)
        : var_(const_cast<atomic<T>&>(var))
        , info_(info)
    {
    }

    T load(memory_order mo = mo_seq_cst) const noexcept
    {
        return var_.load(mo, info_);
    }

    operator T () const noexcept
    {
        return load();
    }

protected:
    atomic<T>& var_;
    debug_info info_;

    atomic_proxy_const& operator = (atomic_proxy_const const&);
};




template<typename T>
class atomic_proxy : public atomic_proxy_const<T>
{
public:
    typedef typename atomic_add_type<T>::type add_type;

    atomic_proxy(atomic<T> /*volatile*/& var, debug_info_param info)
        : atomic_proxy_const<T>(var, info)
    {
    }

    void store(T value, memory_order mo = mo_seq_cst) noexcept
    {
        this->var_.store(value, mo, this->info_);
    }

    bool compare_exchange_weak(T& cmp, T xchg, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.compare_exchange(bool_t<true>(), cmp, xchg, mo, this->info_);
    }

    bool compare_exchange_weak(T& cmp, T xchg, memory_order mo, memory_order failure_mo) noexcept
    {
        return this->var_.compare_exchange(bool_t<true>(), cmp, xchg, mo, failure_mo, this->info_);
    }

    bool compare_exchange_strong(T& cmp, T xchg, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.compare_exchange(bool_t<false>(), cmp, xchg, mo, this->info_);
    }

    bool compare_exchange_strong(T& cmp, T xchg, memory_order mo, memory_order failure_mo) noexcept
    {
        return this->var_.compare_exchange(bool_t<false>(), cmp, xchg, mo, failure_mo, this->info_);
    }

    T exchange(T xchg, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_swap>(), xchg, mo, this->info_);
    }

    T fetch_add(add_type value, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_add>(), value, mo, this->info_);
    }

    T fetch_sub(add_type value, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_sub>(), value, mo, this->info_);
    }

    T fetch_and(T value, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_and>(), value, mo, this->info_);
    }

    T fetch_or(T value, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_or>(), value, mo, this->info_);
    }

    T fetch_xor(T value, memory_order mo = mo_seq_cst) noexcept
    {
        return this->var_.rmw(rmw_type_t<rmw_type_xor>(), value, mo, this->info_);
    }

    T operator = (T value) noexcept
    {
        store(value);
        return value;
    }

    T operator ++ (int) noexcept
    {
        return fetch_add(1);
    }

    T operator -- (int) noexcept
    {
        return fetch_sub(1);
    }

    T operator ++ () noexcept
    {
        return fetch_add(1) + 1;
    }

    T operator -- () noexcept
    {
        return fetch_sub(1) - 1;
    }

    T operator += (add_type value) noexcept
    {
        return fetch_add(value) + value;
    }

    T operator -= (add_type value) noexcept
    {
        return fetch_sub(value) + value;
    }

    T operator &= (T value) noexcept
    {
        return fetch_and(value) & value;
    }

    T operator |= (T value) noexcept
    {
        return fetch_or(value) | value;
    }

    T operator ^= (T value) noexcept
    {
        return fetch_xor(value) ^ value;
    }
    
#if __cplusplus >= 202002L
    void wait(T old, memory_order mo = mo_seq_cst) const noexcept
    {
        return this->var_.wait(old, mo, this->info_);
    }

    void notify_one() noexcept
    {
        return this->var_.notify_one(this->info_);
    }

    void notify_all() noexcept
    {
        return this->var_.notify_all(this->info_);
    }
#endif
};




template<typename T, bool strong_init>
class generic_atomic
{
public:
    generic_atomic()
    {
        context& c = ctx();
        RL_VERIFY(false == c.invariant_executing);
        impl_ = c.atomic_ctor(this);
        initialized_ = false;
        value_ = T();
        already_failed_ = false;

        memset(history_, 0, sizeof(history_));

        if (val(strong_init))
        {
            unsigned const index = c.threadx_->atomic_init(impl_);
            last_index_ = index;
            initialized_ = true;
            history_[index] = T();
            value_ = T();
        }
    }

    ~generic_atomic()
    {
        context& c = ctx();
        RL_VERIFY(false == c.invariant_executing);
        sign_.check($);
        c.atomic_dtor(impl_);
    }

    T debug_value() const
    {
        sign_.check($);
        return value_;
    }

    RL_INLINE
    T load(memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) const noexcept
    {
        RL_VERIFY(mo_release != mo);
        RL_VERIFY(mo_acq_rel != mo);

        switch (mo)
        {
        case mo_relaxed: return load_impl<mo_relaxed, &thread_info_base::atomic_load_relaxed>(info);
        case mo_consume: return load_impl<mo_consume, &thread_info_base::atomic_load_acquire>(info);
        case mo_acquire: return load_impl<mo_acquire, &thread_info_base::atomic_load_acquire>(info);
        case mo_seq_cst: return load_impl<mo_seq_cst, &thread_info_base::atomic_load_seq_cst>(info);
        default: break;
        }

        RL_VERIFY(false);
        return T();
    }

    RL_INLINE
    void store(T v, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        RL_VERIFY(mo_acquire != mo);
        RL_VERIFY(mo_acq_rel != mo);

        switch (mo)
        {
        case mo_relaxed: return store_impl<mo_relaxed, &thread_info_base::atomic_store_relaxed>(v, info);
        case mo_release: return store_impl<mo_release, &thread_info_base::atomic_store_release>(v, info);
        case mo_seq_cst: return store_impl< mo_seq_cst, &thread_info_base::atomic_store_seq_cst>(v, info);
        default: break;
        }

        RL_VERIFY(false);
    }

    RL_INLINE
    bool compare_exchange_weak(T& cmp, T xchg, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<true>(), cmp, xchg, mo, info);
    }

    RL_INLINE
    bool compare_exchange_strong(T& cmp, T xchg, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<false>(), cmp, xchg, mo, info);
    }

    RL_INLINE
    bool compare_exchange_weak(T& cmp, T xchg, memory_order mo, memory_order failure_mo, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<true>(), cmp, xchg, mo, failure_mo, info);
    }

    RL_INLINE
    bool compare_exchange_strong(T& cmp, T xchg, memory_order mo, memory_order failure_mo, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<false>(), cmp, xchg, mo, failure_mo, info);
    }

    // Below overloads support RELACY_ENABLE_MEMORY_ORDER_DEBUG_INFO_DEFAULTING mode,
    // where the author writes 'a.compare_exchange_weak(cmp, 1, std::memory_order_seq_cst, std::memory_order_seq_cst)'
    // and relies on '$' being inserted via the memory order macros.
    RL_INLINE
    bool compare_exchange_weak(T& cmp, T xchg, memory_order mo, debug_info_param info, memory_order failure_mo, debug_info_param DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<true>(), cmp, xchg, mo, failure_mo, info);
    }

    RL_INLINE
    bool compare_exchange_strong(T& cmp, T xchg, memory_order mo, debug_info_param info, memory_order failure_mo, debug_info_param DEFAULTED_DEBUG_INFO) noexcept
    {
        return compare_exchange(bool_t<false>(), cmp, xchg, mo, failure_mo, info);
    }

    template<bool spurious_failures>
    RL_INLINE
    bool compare_exchange(bool_t<spurious_failures>, T& cmp, T xchg, memory_order mo, debug_info_param info) noexcept
    {
        switch (mo)
        {
        case mo_relaxed: return compare_swap_impl<spurious_failures, mo_relaxed, &thread_info_base::atomic_rmw_relaxed, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
        case mo_consume: return compare_swap_impl<spurious_failures, mo_consume, &thread_info_base::atomic_rmw_acquire, mo_consume, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
        case mo_acquire: return compare_swap_impl<spurious_failures, mo_acquire, &thread_info_base::atomic_rmw_acquire, mo_acquire, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
        case mo_release: return compare_swap_impl<spurious_failures, mo_release, &thread_info_base::atomic_rmw_release, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
        case mo_acq_rel: return compare_swap_impl<spurious_failures, mo_acq_rel, &thread_info_base::atomic_rmw_acq_rel, mo_acquire, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
        case mo_seq_cst: return compare_swap_impl<spurious_failures, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst, mo_seq_cst, &thread_info_base::atomic_load_seq_cst_rmw>(cmp, xchg, info);
        }

        RL_VERIFY(false);
        return false;
    }

    template<bool spurious_failures>
    RL_INLINE
    bool compare_exchange(bool_t<spurious_failures>, T& cmp, T xchg, memory_order mo, memory_order failure_mo, debug_info_param info)
    {
        switch (mo)
        {
        case mo_relaxed:
            {
                RL_VERIFY(mo_relaxed == failure_mo);
                return compare_swap_impl<spurious_failures, mo_relaxed, &thread_info_base::atomic_rmw_relaxed, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
            }
        case mo_consume:
            {
                RL_VERIFY(mo_relaxed == failure_mo || mo_consume == failure_mo);
                switch (failure_mo)
                {
                case mo_relaxed: return compare_swap_impl<spurious_failures, mo_consume, &thread_info_base::atomic_rmw_acquire, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
                case mo_consume: return compare_swap_impl<spurious_failures, mo_consume, &thread_info_base::atomic_rmw_acquire, mo_consume, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                default: RL_VERIFY(false); return false;
                }
            }
        case mo_acquire:
            {
                RL_VERIFY(mo_relaxed == failure_mo || mo_consume == failure_mo || mo_acquire == failure_mo);
                switch (failure_mo)
                {
                case mo_relaxed: return compare_swap_impl<spurious_failures, mo_acquire, &thread_info_base::atomic_rmw_acquire, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
                case mo_consume: return compare_swap_impl<spurious_failures, mo_acquire, &thread_info_base::atomic_rmw_acquire, mo_consume, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                case mo_acquire: return compare_swap_impl<spurious_failures, mo_acquire, &thread_info_base::atomic_rmw_acquire, mo_acquire, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                default: RL_VERIFY(false); return false;
                }
            }
        case mo_release:
            {
                RL_VERIFY(mo_relaxed == failure_mo);
                return compare_swap_impl<spurious_failures, mo_release, &thread_info_base::atomic_rmw_release, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
            }
        case mo_acq_rel:
            {
                RL_VERIFY(mo_relaxed == failure_mo || mo_consume == failure_mo || mo_acquire == failure_mo);
                switch (failure_mo)
                {
                case mo_relaxed: return compare_swap_impl<spurious_failures, mo_acq_rel, &thread_info_base::atomic_rmw_acq_rel, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
                case mo_consume: return compare_swap_impl<spurious_failures, mo_acq_rel, &thread_info_base::atomic_rmw_acq_rel, mo_consume, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                case mo_acquire: return compare_swap_impl<spurious_failures, mo_acq_rel, &thread_info_base::atomic_rmw_acq_rel, mo_acquire, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                default: RL_VERIFY(false); return false;
                }
            }
        case mo_seq_cst:
            {
                RL_VERIFY(mo_relaxed == failure_mo || mo_consume == failure_mo || mo_acquire == failure_mo || mo_seq_cst == failure_mo);
                switch (failure_mo)
                {
                case mo_relaxed: return compare_swap_impl<spurious_failures, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst, mo_relaxed, &thread_info_base::atomic_load_relaxed_rmw>(cmp, xchg, info);
                case mo_consume: return compare_swap_impl<spurious_failures, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst, mo_consume, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                case mo_acquire: return compare_swap_impl<spurious_failures, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst, mo_acquire, &thread_info_base::atomic_load_acquire_rmw>(cmp, xchg, info);
                case mo_seq_cst: return compare_swap_impl<spurious_failures, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst, mo_seq_cst, &thread_info_base::atomic_load_seq_cst_rmw>(cmp, xchg, info);
                default: RL_VERIFY(false); return false;
                }
            }
        }

        RL_VERIFY(false);
        return false;
    }

    T exchange(T xchg, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_swap>(), xchg, mo, info);
    }

    T fetch_add(typename atomic_add_type<T>::type value, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_add>(), value, mo, info);
    }

    T fetch_sub(typename atomic_add_type<T>::type value, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_sub>(), value, mo, info);
    }

    T fetch_and(T value, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_and>(), value, mo, info);
    }

    T fetch_or(T value, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_or>(), value, mo, info);
    }

    T fetch_xor(T value, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept
    {
        return rmw(rmw_type_t<rmw_type_xor>(), value, mo, info);
    }

    template<typename Y, rmw_type_e type>
    RL_INLINE
    T rmw(rmw_type_t<type>, Y op, memory_order mo, debug_info_param info)
    {
        switch (mo)
        {
        case mo_relaxed: return rmw_impl<Y, mo_relaxed, &thread_info_base::atomic_rmw_relaxed>(rmw_type_t<type>(), op, info);
        case mo_consume: return rmw_impl<Y, mo_consume, &thread_info_base::atomic_rmw_acquire>(rmw_type_t<type>(), op, info);
        case mo_acquire: return rmw_impl<Y, mo_acquire, &thread_info_base::atomic_rmw_acquire>(rmw_type_t<type>(), op, info);
        case mo_release: return rmw_impl<Y, mo_release, &thread_info_base::atomic_rmw_release>(rmw_type_t<type>(), op, info);
        case mo_acq_rel: return rmw_impl<Y, mo_acq_rel, &thread_info_base::atomic_rmw_acq_rel>(rmw_type_t<type>(), op, info);
        case mo_seq_cst: return rmw_impl<Y, mo_seq_cst, &thread_info_base::atomic_rmw_seq_cst>(rmw_type_t<type>(), op, info);
        }

        RL_VERIFY(false);
        return T();
    }

    // Internal wait/wake methods for futex-style operations (used by pthread.hpp)
    unpark_reason wait(context& c, bool is_timed, bool allow_spurious_wakeup, debug_info_param info)
    {
        sign_.check(info);
        return c.threadx_->atomic_wait(impl_, is_timed, allow_spurious_wakeup, info);
    }

    thread_id_t wake(context& c, thread_id_t count, debug_info_param info)
    {
        sign_.check(info);
        return c.threadx_->atomic_wake(impl_, count, info);
    }

#if __cplusplus >= 202002L
    void wait(T expected, memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info = std::source_location::current()) const noexcept
    {
        context& c = ctx();
        sign_.check(info);
        
        for (;;)
        {
            // See rl_int_futex_impl for a similar implementation.

            c.sched();
            c.atomic_thread_fence_seq_cst();

            T current;
            {
                preemption_disabler pd(c);
                current = this->load(mo, info);
            }
            
            // If value has changed, we're done
            if (current != expected)
                break;
            
            // Value still matches expected, so park (this may spuriously wake)
            // Park allows scheduling and may wake spuriously or due to notify
            const_cast<generic_atomic*>(this)->wait(c, false, true, info);
            
            // After waking (spurious or real), loop back to check again
        }
    }

    void notify_one(debug_info_param info = std::source_location::current()) noexcept
    {
        context& c = ctx();
        c.sched();
        c.atomic_thread_fence_seq_cst();
        
        // Wake one waiting thread from the waitset
        this->wake(c, 1, info);
    }

    void notify_all(debug_info_param info = std::source_location::current()) noexcept
    {
        context& c = ctx();
        c.sched();
        
        // Wake all waiting threads from the waitset
        this->wake(c, thread_id_t(-1), info);
    }
#endif

private:
    T value_;
    T history_ [atomic_history_size];
    atomic_data* impl_;
    unsigned last_index_;
    signature<987654321> sign_;
    bool initialized_;
    bool already_failed_;

    template<memory_order mo, unsigned (thread_info_base::*impl)(atomic_data* RL_RESTRICT data)>
    T load_impl(debug_info_param info) const
    {
        context& c = ctx();
        c.sched();
        sign_.check(info);

        if (false == c.invariant_executing)
        {
            unsigned const index = (c.threadx_->*impl)(impl_);
            if ((unsigned)-1 == index)
            {
                RL_HIST(atomic_load_event<T>) {this, T(), mo, false} RL_HIST_END();
                RL_ASSERT_IMPL(false, test_result_unitialized_access, "", info);
            }
            T const v = history_[index];

            RL_HIST(atomic_load_event<T>) {this, v, mo, last_index_ != index} RL_HIST_END();

            return v;
        }
        else
        {
            if (false == initialized_)
            {
                RL_HIST(atomic_load_event<T>) {this, T(), mo, false} RL_HIST_END();
                RL_ASSERT_IMPL(false, test_result_unitialized_access, "", info);
            }
            return value_;
        }
    }

    template<memory_order mo, unsigned (thread_info_base::*impl)(atomic_data* RL_RESTRICT data)>
    void store_impl(T v, debug_info_param info)
    {
        context& c = ctx();
        RL_VERIFY(false == c.invariant_executing);
        c.sched();
        sign_.check(info);

        unsigned const index = (c.threadx_->*impl)(impl_);

        T const prev = value_;
        last_index_ = index;
        history_[index] = v;
        value_ = v;
        initialized_ = true;
        RL_HIST(atomic_store_event<T>) {this, prev, v, mo} RL_HIST_END();
    }

    template<bool spurious_failures, memory_order mo, unsigned (thread_info_base::*impl)(atomic_data* RL_RESTRICT data, bool&), memory_order failure_mo, unsigned (thread_info_base::*failure_impl)(atomic_data* RL_RESTRICT data)>
    bool compare_swap_impl(T& cmp, T xchg, debug_info_param info)
    {
        context& c = ctx();
        RL_VERIFY(false == c.invariant_executing);
        c.sched();
        sign_.check(info);

        if (false == initialized_)
        {
            RL_HIST(atomic_load_event<T>) {this, T(), mo, false} RL_HIST_END();
            RL_ASSERT_IMPL(false, test_result_unitialized_access, "", info);
        }

        bool success = false;
        bool spurious_failure = false;
        bool aba = false;

        T const cmpv = cmp;
        T const current = value_;
        if (current == cmpv)
        {
            if (val(spurious_failures))
            {
                if (c.is_random_sched())
                {
                    spurious_failure = (0 == c.rand(4, sched_type_cas_fail));
                }
                else
                {
                    if (false == already_failed_)
                    {
                        spurious_failure = 0 == c.rand(2, sched_type_cas_fail);
                        if (spurious_failure)
                            already_failed_ = true;
                    }
                }
            }

            if (false == spurious_failure)
            {
                success = true;
                unsigned const index = (c.threadx_->*impl)(impl_, aba);
                value_ = xchg;
                last_index_ = index;
                history_[index] = xchg;
            }
        }

        if (false == success)
        {
            (c.threadx_->*failure_impl)(impl_);
            cmp = current;
        }

        RL_HIST(atomic_cas_event<T>) {RL_INFO, this, current, cmpv, xchg, mo, success, spurious_failure, aba} RL_HIST_END();

        return success;
    }

    template<typename Y, memory_order mo, unsigned (thread_info_base::*impl)(atomic_data* RL_RESTRICT data, bool&), rmw_type_e type>
    T rmw_impl(rmw_type_t<type>, Y op, debug_info_param info)
    {
        context& c = ctx();
        RL_VERIFY(false == c.invariant_executing);
        c.sched();
        sign_.check(info);

        if (false == initialized_)
        {
            RL_HIST(atomic_load_event<T>) {this, T(), mo, false} RL_HIST_END();
            RL_ASSERT_IMPL(false, test_result_unitialized_access, "", info);
        }

        bool aba;
        unsigned const index = (c.threadx_->*impl)(impl_, aba);

        T const prev_value = value_;
        T const new_value = perform_rmw(rmw_type_t<type>(), prev_value, op);
        value_ = new_value;
        last_index_ = index;
        history_[index] = new_value;

        typedef atomic_rmw_event<T, Y> atomic_rmw_event_t;
        RL_HIST(atomic_rmw_event_t) {RL_INFO, this, prev_value, op, new_value, mo, type} RL_HIST_END();

        return prev_value;
    }

    RL_NOCOPY(generic_atomic);
};





template<typename T>
#if __cplusplus >= 202002L
// atomic's default constructor value initializes since C++20
class atomic : public generic_atomic<T, true>
#else
class atomic : public generic_atomic<T, false>
#endif
{
public:
    atomic() noexcept = default;

    /*explicit*/ atomic(T value) noexcept (std::is_nothrow_default_constructible<T>::value)
    {
        this->store(value, mo_relaxed, $);
    }

    atomic_proxy_const<T> operator () (debug_info_param info) const noexcept /*volatile*/
    {
        return atomic_proxy_const<T>(*this, info);
    }

    atomic_proxy<T> operator () (debug_info_param info) noexcept /*volatile*/
    {
        return atomic_proxy<T>(*this, info);
    }

    operator T () const noexcept
    {
        return (*this)($).load();
    }

    bool is_lock_free() const noexcept /*volatile*/
    {
        return true;
    }

    friend class atomic_proxy<T>;
    friend class atomic_proxy_const<T>;

    RL_NOCOPY(atomic);
};


typedef atomic<bool> atomic_bool;
typedef atomic<void*> atomic_address;

typedef atomic<char> atomic_char;
typedef atomic<signed char> atomic_schar;
typedef atomic<unsigned char> atomic_uchar;
typedef atomic<short> atomic_short;
typedef atomic<unsigned short> atomic_ushort;
typedef atomic<int> atomic_int;
typedef atomic<unsigned int> atomic_uint;
typedef atomic<long> atomic_long;
typedef atomic<unsigned long> atomic_ulong;
typedef atomic<long long> atomic_llong;
typedef atomic<unsigned long long> atomic_ullong;
//typedef atomic<char16_t> atomic_char16_t;
//typedef atomic<char32_t> atomic_char32_t;
typedef atomic<wchar_t> atomic_wchar_t;

//typedef atomic<int_least8_t> atomic_int_least8_t;
//typedef atomic<uint_least8_t> atomic_uint_least8_t;
//typedef atomic<int_least16_t> atomic_int_least16_t;
//typedef atomic<uint_least16_t> atomic_uint_least16_t;
//typedef atomic<int_least32_t> atomic_int_least32_t;
//typedef atomic<uint_least32_t> atomic_uint_least32_t;
//typedef atomic<int_least64_t> atomic_int_least64_t;
//typedef atomic<uint_least64_t> atomic_uint_least64_t;
//typedef atomic<int_fast8_t> atomic_int_fast8_t;
//typedef atomic<uint_fast8_t> atomic_uint_fast8_t;
//typedef atomic<int_fast16_t> atomic_int_fast16_t;
//typedef atomic<uint_fast16_t> atomic_uint_fast16_t;
//typedef atomic<int_fast32_t> atomic_int_fast32_t;
//typedef atomic<uint_fast32_t> atomic_uint_fast32_t;
//typedef atomic<int_fast64_t> atomic_int_fast64_t;
//typedef atomic<uint_fast64_t> atomic_uint_fast64_t;
typedef atomic<intptr_t> atomic_intptr_t;
typedef atomic<uintptr_t> atomic_uintptr_t;
typedef atomic<size_t> atomic_size_t;
//typedef atomic<ssize_t> atomic_ssize_t;
typedef atomic<ptrdiff_t> atomic_ptrdiff_t;
//typedef atomic<intmax_t> atomic_intmax_t;
//typedef atomic<uintmax_t> atomic_uintmax_t;




template<thread_id_t thread_count>
struct atomic_data_impl : atomic_data
{
    typedef thread_info<thread_count> thread_info_t;

    struct history_record
    {
        timestamp_t acq_rel_order_ [thread_count];
        timestamp_t first_seen_order_ [thread_count];

        bool busy_;
        bool seq_cst_;
        thread_id_t thread_id_;
        timestamp_t acq_rel_timestamp_;
    };

    static size_t const history_size = atomic_history_size;
    aligned<history_record> history_ [history_size];
    unsigned current_index_;
    waitset<thread_count> futex_ws_;
    sync_var<thread_count> futex_sync_;

    atomic_data_impl()
    {
        memset(history_, 0, sizeof(history_));

        current_index_ = 0;
        history_record& rec = history_[0];
        history_[atomic_history_size - 1].busy_ = false;

        rec.busy_ = false;
        rec.seq_cst_ = false;
        rec.thread_id_ = (thread_id_t)-1;
    }

    atomic_data_impl(thread_info_t& th)
    {
        current_index_ = 0;
        history_[atomic_history_size - 1].busy_ = false;

        history_record& rec = history_[0];
        rec.busy_ = true;
        rec.seq_cst_ = false;
        rec.thread_id_ = th.index_;

        th.own_acq_rel_order_ += 1;
        rec.acq_rel_timestamp_ = th.own_acq_rel_order_;

        foreach<thread_count>(rec.acq_rel_order_, assign_zero);
        foreach<thread_count>(rec.first_seen_order_, assign<(timestamp_t)-1>);
        rec.first_seen_order_[th.index_] = th.own_acq_rel_order_;
    }
};

struct atomic_flag {
    
    struct _private_constructor_tag {};

    atomic_flag& operator=(const atomic_flag&) = delete;
    
    atomic_flag() noexcept = default;
    
#if __cplusplus >= 201703L
    atomic_flag(const atomic_flag&) = delete;
#else
    // Before C++17's guaranteed copy elision, we allow the copy ctor to be called
    // so that ATOMIC_FLAG_INIT can work correctly.
    atomic_flag(const atomic_flag& other) noexcept {
        flg.store(other.flg.load(mo_relaxed, $), mo_relaxed, $);
    }
#endif
    
    // Not standard; used to support ATOMIC_FLAG_INIT
    atomic_flag(_private_constructor_tag, bool initial) noexcept : flg(initial) {}

    void clear(memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) noexcept {
        flg.store(false, mo, info);
    }

    bool test_and_set(memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) {
        return flg.exchange(true, mo, info);
    }

#if __cplusplus >= 202002L

    bool test(memory_order mo DEFAULTED_ATOMIC_OP_MO, debug_info_param info DEFAULTED_DEBUG_INFO) {
        return flg.load(mo, info);
    }

    void wait(bool old, memory_order mo = mo_seq_cst, debug_info_param info = std::source_location::current()) const noexcept
    {
        flg.wait(old, mo, info);
    }

    void notify_one(debug_info_param info = std::source_location::current()) noexcept
    {
        flg.notify_one(info);
    }

    void notify_all(debug_info_param info = std::source_location::current()) noexcept
    {
        flg.notify_all(info);
    }

#endif

private:
    rl::atomic<bool> flg;

};

// Note: std::atomic_flag is an aggregate type (at least in libstdc++), allowing it to be
// initialized with a brace-enclosed initializer. However, to allow implementing rl::atomic_flag
// in terms of rl::atomic, rl::atomic_flag is not an aggregate.
#define RL_ATOMIC_FLAG_INIT rl::atomic_flag(rl::atomic_flag::_private_constructor_tag{}, false)

}



#endif

