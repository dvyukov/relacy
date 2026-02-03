#pragma once

#include "../relacy/relacy.hpp"



struct tls_basic_test : rl::test_suite<tls_basic_test, 3>
{
    rl::thread_local_var<unsigned> x;

    void thread(unsigned index)
    {
        RL_ASSERT(x.get($) == 0);
        x.set(index + 10, $);
        RL_ASSERT(x.get($) == index + 10);
    }
};


struct tls_basic_test2 : rl::test_suite<tls_basic_test2, 3>
{
    TLS_T(unsigned) x;

    void thread(unsigned index)
    {
        RL_ASSERT(VAR(x) == 0);
        VAR(x) = index + 10;
        RL_ASSERT(VAR(x) == index + 10);
    }
};


struct tls_reset_test : rl::test_suite<tls_reset_test, 3, rl::test_result_user_assert_failed>
{
    rl::thread_local_var<unsigned> x;

    void thread(unsigned index)
    {
        RL_ASSERT(x.get($) == 0);
        x.set(index + 10, $);
        RL_ASSERT(x.get($) == index + 10);
        RL_ASSERT(false);
    }
};


rl::thread_local_var<unsigned> tls_global_test_x;
struct tls_global_test : rl::test_suite<tls_global_test, 3, rl::test_result_user_assert_failed>
{
    void thread(unsigned index)
    {
        RL_ASSERT(tls_global_test_x.get($) == 0);
        tls_global_test_x.set(index + 10, $);
        RL_ASSERT(tls_global_test_x.get($) == index + 10);
        RL_ASSERT(false);
    }
};


struct tls_win32_test : rl::test_suite<tls_win32_test, 3>
{
    unsigned long slot;

    void before()
    {
        slot = TlsAlloc();
    }

    void after()
    {
        TlsFree(slot);
    }

    void thread(unsigned index)
    {
        RL_ASSERT(TlsGetValue(slot) == 0);
        TlsSetValue(slot, (void*)(uintptr_t)(index + 10));
        RL_ASSERT(TlsGetValue(slot) == (void*)(uintptr_t)(index + 10));
    }
};

struct complex_init
#ifdef __cpp_guaranteed_copy_elision
: rl::nocopy<>
#endif
{
    explicit complex_init(unsigned x) : value(x) {}

    unsigned value = 0;
};

struct tls_cxx_test : rl::test_suite<tls_cxx_test, 3>
{
  rl::cxx_thread_local_var<complex_init> x;

  void thread(unsigned index)
  {
    if (index == 3) {
        return; // check that not accessing the tls is fine
    }
    auto factory = [&] { return complex_init{index + 2}; };
    auto& v1 = x.get(factory, $);
    RL_ASSERT(index + 2 == v1.value);

    v1.value = index + 5;
    RL_ASSERT(index + 5 == x.get(factory, $).value);
  }
};
