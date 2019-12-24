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


#include "relacy.hpp"


namespace std
{
    namespace rlimp 
    {
      using namespace rl;
    }

#    define memory_order rlimp::memory_order
#    define mo_relaxed rlimp::mo_relaxed
#    define mo_consume rlimp::mo_consume
#    define mo_acquire rlimp::mo_acquire
#    define mo_release rlimp::mo_release
#    define mo_acq_rel rlimp::mo_acq_rel
#    define mo_seq_cst rlimp::mo_seq_cst

#    define atomic rlimp::atomic
#    define atomic_thread_fence rlimp::atomic_thread_fence
#    define atomic_signal_fence rlimp::atomic_signal_fence

#    define atomic_bool rlimp::atomic_bool
#    define atomic_address rlimp::atomic_address

#    define atomic_char rlimp::atomic_char
#    define atomic_schar rlimp::atomic_schar
#    define atomic_uchar rlimp::atomic_uchar
#    define atomic_short rlimp::atomic_short
#    define atomic_ushort rlimp::atomic_ushort
#    define atomic_int rlimp::atomic_int
#    define atomic_uint rlimp::atomic_uint
#    define atomic_long rlimp::atomic_long
#    define atomic_ulong rlimp::atomic_ulong
#    define atomic_llong rlimp::atomic_llong
#    define atomic_ullong rlimp::atomic_ullong
//#    define atomic_char16_t rlimp::atomic_char16_t
//#    define atomic_char32_t rlimp::atomic_char32_t
#    define atomic_wchar_t rlimp::atomic_wchar_t

//#    define atomic_int_least8_t rlimp::atomic_int_least8_t
//#    define atomic_uint_least8_t rlimp::atomic_uint_least8_t
//#    define atomic_int_least16_t rlimp::atomic_int_least16_t
//#    define atomic_uint_least16_t rlimp::atomic_uint_least16_t
//#    define atomic_int_least32_t rlimp::atomic_int_least32_t
//#    define atomic_uint_least32_t rlimp::atomic_uint_least32_t
//#    define atomic_int_least64_t rlimp::atomic_int_least64_t
//#    define atomic_uint_least64_t rlimp::atomic_uint_least64_t
//#    define atomic_int_fast8_t rlimp::atomic_int_fast8_t
//#    define atomic_uint_fast8_t rlimp::atomic_uint_fast8_t
//#    define atomic_int_fast16_t rlimp::atomic_int_fast16_t
//#    define atomic_uint_fast16_t rlimp::atomic_uint_fast16_t
//#    define atomic_int_fast32_t rlimp::atomic_int_fast32_t
//#    define atomic_uint_fast32_t rlimp::atomic_uint_fast32_t
//#    define atomic_int_fast64_t rlimp::atomic_int_fast64_t
//#    define atomic_uint_fast64_t rlimp::atomic_uint_fast64_t
#    define atomic_intptr_t rlimp::atomic_intptr_t
#    define atomic_uintptr_t rlimp::atomic_uintptr_t
#    define atomic_size_t rlimp::atomic_size_t
//#    define atomic_ssize_t rlimp::atomic_ssize_t
#    define atomic_ptrdiff_t rlimp::atomic_ptrdiff_t
//#    define atomic_intmax_t rlimp::atomic_intmax_t
//#    define atomic_uintmax_t mutex rlimp::atomic_uintmax_t

#    define mutex rlimp::mutex
#    define recursive_mutex rlimp::recursive_mutex
#    define condition_variable rlimp::condition_variable
#    define condition_variable_any rlimp::condition_variable_any
}


namespace rl
{
    namespace rlimp
    {
        using namespace rl;
    }
}

#endif
