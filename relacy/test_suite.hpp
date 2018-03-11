/*  Relacy Race Detector
 *  Copyright (c) 2008-2013, Dmitry S. Vyukov
 *  All rights reserved.
 *  This software is provided AS-IS with no warranty, either express or implied.
 *  This software is distributed under a license and may not be copied,
 *  modified or distributed except as expressly authorized under the
 *  terms of the license contained in the file LICENSE in this distribution.
 */

#ifndef RL_TEST_SUITE_HPP
#define RL_TEST_SUITE_HPP
#ifdef _MSC_VER
#   pragma once
#endif

#include "base.hpp"
#include "test_result.hpp"


namespace rl
{


template<
    typename derived_t,
    test_result_e result = test_result_success>
struct test_suite : nocopy<>
{
    struct params
    {
        static test_result_e const expected_result = result;
    };

    void invariant() {}
    void before() {}
    void after() {}
};


}

#endif
