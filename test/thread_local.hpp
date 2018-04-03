#pragma once

#include "../relacy/relacy.hpp"



struct tls_basic_test
{
    rl::thread_local_var<unsigned> x;

    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        RL_ASSERT(x.get($) == 0);
        x.set(index + 10, $);
        RL_ASSERT(x.get($) == index + 10);
    }
};


struct tls_basic_test2
{
    TLS_T(unsigned) x;

    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        RL_ASSERT(VAR(x) == 0);
        VAR(x) = index + 10;
        RL_ASSERT(VAR(x) == index + 10);
    }
};


struct tls_reset_test
{
    rl::thread_local_var<unsigned> x;

    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        RL_ASSERT(x.get($) == 0);
        x.set(index + 10, $);
        RL_ASSERT(x.get($) == index + 10);
        RL_ASSERT(false);
    }
};


rl::thread_local_var<unsigned> tls_global_test_x;
struct tls_global_test
{
    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        RL_ASSERT(tls_global_test_x.get($) == 0);
        tls_global_test_x.set(index + 10, $);
        RL_ASSERT(tls_global_test_x.get($) == index + 10);
        RL_ASSERT(false);
    }
};


struct tls_win32_test
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

    void invariant() { }

    void thread(unsigned index)
    {
        RL_ASSERT(TlsGetValue(slot) == 0);
        TlsSetValue(slot, (void*)(uintptr_t)(index + 10));
        RL_ASSERT(TlsGetValue(slot) == (void*)(uintptr_t)(index + 10));
    }
};
