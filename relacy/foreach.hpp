/*  Relacy Race Detector
 *  Copyright (c) 2008-2013, Dmitry S. Vyukov
 *  All rights reserved.
 *  This software is provided AS-IS with no warranty, either express or implied.
 *  This software is distributed under a license and may not be copied,
 *  modified or distributed except as expressly authorized under the
 *  terms of the license contained in the file LICENSE in this distribution.
 */

#ifndef RL_FOREACH_HPP
#define RL_FOREACH_HPP
#ifdef _MSC_VER
#   pragma once
#endif

#include "base.hpp"

namespace rl
{

template<typename T, typename F>
RL_INLINE void foreach(
    thread_id_t count,
    T* v1,
    F func)
{
    for (thread_id_t i = 0; i < count; i++)
    {
        (*func)(v1[i]);
    }
}

template<typename T>
RL_INLINE void foreach(
    thread_id_t count,
    T* v1, T* v2,
    void (*func)(T& e1, T& e2))
{
    for (thread_id_t i = 0; i < count; i++)
    {
        (*func)(v1[i], v2[i]);
    }
}

template<typename T>
RL_INLINE void foreach(
    thread_id_t count,
    T* v1, T* v2, T* v3,
    void (*func)(T& e1, T& e2, T& e3))
{
    for (thread_id_t i = 0; i < count; i++)
    {
        (*func)(v1[i], v2[i], v3[i]);
    }
}

RL_INLINE void assign_zero(timestamp_t& elem)
{
    elem = 0;
}

RL_INLINE void assign_zero_u(unsigned& elem)
{
    elem = 0;
}

template<timestamp_t value>
RL_INLINE void assign(timestamp_t& elem)
{
    elem = value;
}

RL_INLINE void assign(timestamp_t& elem1, timestamp_t& elem2)
{
    elem1 = elem2;
}

RL_INLINE void assign_max(timestamp_t& elem1, timestamp_t& elem2)
{
    if (elem2 > elem1)
        elem1 = elem2;
}

RL_INLINE void plus_one(timestamp_t& elem)
{
    elem += 1;
}

}


#endif
