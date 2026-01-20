/*  Relacy Race Detector
 *  Copyright (c) 2008-2013, Dmitry S. Vyukov
 *  All rights reserved.
 *  This software is provided AS-IS with no warranty, either express or implied.
 *  This software is distributed under a license and may not be copied,
 *  modified or distributed except as expressly authorized under the
 *  terms of the license contained in the file LICENSE in this distribution.
 */

#ifndef RL_RELACY_STD_HPP
#define RL_RELACY_STD_HPP
#ifdef _MSC_VER
#   pragma once
#endif

#if defined(_GLIBCXX_RELEASE) || defined(_LIBCPP_VERSION)
#error "Detected that standard library headers were included before relacy_std.hpp - \
please ensure relacy_std.hpp is the first header included in the .cpp. \
See https://github.com/dvyukov/relacy/blob/master/README.md#overriding-std-types."
#endif

#include "fakestd/prologue.hpp"

#include "relacy.hpp"


namespace std
{
    using rl::memory_order;
    
#ifndef RELACY_ENABLE_MEMORY_ORDER_DEBUG_INFO_DEFAULTING
    using rl::memory_order_relaxed;
    using rl::memory_order_consume;
    using rl::memory_order_acquire;
    using rl::memory_order_release;
    using rl::memory_order_acq_rel;
    using rl::memory_order_seq_cst;
#endif

    using rl::mo_relaxed;
    using rl::mo_consume;
    using rl::mo_acquire;
    using rl::mo_release;
    using rl::mo_acq_rel;
    using rl::mo_seq_cst;

    using rl::atomic;
    using rl::atomic_thread_fence;
    using rl::atomic_signal_fence;

    using rl::atomic_bool;
    using rl::atomic_address;

    using rl::atomic_char;
    using rl::atomic_schar;
    using rl::atomic_uchar;
    using rl::atomic_short;
    using rl::atomic_ushort;
    using rl::atomic_int;
    using rl::atomic_uint;
    using rl::atomic_long;
    using rl::atomic_ulong;
    using rl::atomic_llong;
    using rl::atomic_ullong;
//    using rl::atomic_char16_t;
//    using rl::atomic_char32_t;
    using rl::atomic_wchar_t;

//    using rl::atomic_int_least8_t;
//    using rl::atomic_uint_least8_t;
//    using rl::atomic_int_least16_t;
//    using rl::atomic_uint_least16_t;
//    using rl::atomic_int_least32_t;
//    using rl::atomic_uint_least32_t;
//    using rl::atomic_int_least64_t;
//    using rl::atomic_uint_least64_t;
//    using rl::atomic_int_fast8_t;
//    using rl::atomic_uint_fast8_t;
//    using rl::atomic_int_fast16_t;
//    using rl::atomic_uint_fast16_t;
//    using rl::atomic_int_fast32_t;
//    using rl::atomic_uint_fast32_t;
//    using rl::atomic_int_fast64_t;
//    using rl::atomic_uint_fast64_t;
    using rl::atomic_intptr_t;
    using rl::atomic_uintptr_t;
    using rl::atomic_size_t;
//    using rl::atomic_ssize_t;
    using rl::atomic_ptrdiff_t;
//    using rl::atomic_intmax_t;
//    using rl::atomic_uintmax_t;

    using rl::mutex;
    using rl::recursive_mutex;
    using rl::condition_variable;
    using rl::condition_variable_any;

    using rl::lock_guard;
    using rl::unique_lock;
}

#endif
