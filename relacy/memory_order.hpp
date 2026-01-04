/*  Relacy Race Detector
 *  Copyright (c) 2008-2013, Dmitry S. Vyukov
 *  All rights reserved.
 *  This software is provided AS-IS with no warranty, either express or implied.
 *  This software is distributed under a license and may not be copied,
 *  modified or distributed except as expressly authorized under the
 *  terms of the license contained in the file LICENSE in this distribution.
 */

#ifndef RL_MEMORY_ORDER_HPP
#define RL_MEMORY_ORDER_HPP
#ifdef _MSC_VER
#   pragma once
#endif

#include "base.hpp"


namespace rl
{

    #if __cplusplus >= 202002L
    
    enum class memory_order : int
    {
        relaxed, consume, acquire, release, acq_rel, seq_cst
    };
    inline constexpr memory_order memory_order_relaxed = memory_order::relaxed;
    inline constexpr memory_order memory_order_consume = memory_order::consume;
    inline constexpr memory_order memory_order_acquire = memory_order::acquire;
    inline constexpr memory_order memory_order_release = memory_order::release;
    inline constexpr memory_order memory_order_acq_rel = memory_order::acq_rel;
    inline constexpr memory_order memory_order_seq_cst = memory_order::seq_cst;

    inline constexpr memory_order mo_relaxed = memory_order_relaxed;
    inline constexpr memory_order mo_consume = memory_order_consume;
    inline constexpr memory_order mo_acquire = memory_order_acquire;
    inline constexpr memory_order mo_release = memory_order_release;
    inline constexpr memory_order mo_acq_rel = memory_order_acq_rel;
    inline constexpr memory_order mo_seq_cst = memory_order_seq_cst;

    #else

    enum memory_order
    {
        memory_order_relaxed,
        memory_order_consume,
        memory_order_acquire,
        memory_order_release,
        memory_order_acq_rel,
        memory_order_seq_cst,
        
        mo_relaxed = memory_order_relaxed,
        mo_consume = memory_order_consume,
        mo_acquire = memory_order_acquire,
        mo_release = memory_order_release,
        mo_acq_rel = memory_order_acq_rel,
        mo_seq_cst = memory_order_seq_cst
    };

    #endif

inline char const* format(memory_order mo)
{
    switch (mo)
    {
    case mo_relaxed: return "relaxed";
    case mo_consume: return "consume";
    case mo_acquire: return "acquire";
    case mo_release: return "release";
    case mo_acq_rel: return "acq_rel";
    case mo_seq_cst: return "seq_cst";
    }
    RL_VERIFY(!"invalid value of memory order");
    throw std::logic_error("invalid value of memory order");
}


}

#endif
