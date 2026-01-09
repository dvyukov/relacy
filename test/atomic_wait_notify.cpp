#if __cplusplus >= 202002L

#include "../relacy/relacy_std.hpp"

struct test_atomic_wait_notify_basic : rl::test_suite<test_atomic_wait_notify_basic, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
            RL_ASSERT(data.load() == 1);
        }
        else
        {
            data.store(1);
            data.notify_one();
        }
    }
};

struct test_atomic_notify_one_insufficient : rl::test_suite<test_atomic_notify_one_insufficient, 3, rl::test_result_deadlock>
{
    std::atomic<int> data;

    void before()
    {
        data($).store(0);
    }

    void thread(unsigned index)
    {
        if (index < 2)
        {
            data($).wait(0);
        }
        else
        {
            data($).store(1);
            data($).notify_one();
        }
    }
};

struct test_atomic_notify_all : rl::test_suite<test_atomic_notify_all, 4>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (index < 3)
        {
            data.wait(0);
        }
        else
        {
            data.store(1);
            data.notify_all();
        }
    }
};

struct test_atomic_wait_no_block : rl::test_suite<test_atomic_wait_no_block, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(1);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
            RL_ASSERT(data.load() == 1);
        }
        else
        {
            RL_ASSERT(data.load() == 1);
        }
    }
};

struct test_atomic_wait_notify_cycles : rl::test_suite<test_atomic_wait_notify_cycles, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
            RL_ASSERT(data.load() == 1);
            
            data.store(2);
            data.notify_one();
        }
        else
        {
            data.store(1);
            data.notify_one();
            
            data.wait(1);
            RL_ASSERT(data.load() == 2);
        }
    }
};

struct test_atomic_notify_no_waiters : rl::test_suite<test_atomic_notify_no_waiters, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.store(1);
            data.notify_one();
            data.store(2);
            data.notify_all();
        }
        else
        {
            while (data.load() < 2) {}
            RL_ASSERT(data.load() == 2);
        }
    }
};

struct test_atomic_wait_memory_order : rl::test_suite<test_atomic_wait_memory_order, 2>
{
    std::atomic<int> flag;
    rl::var<int> shared_data;

    void before()
    {
        flag.store(0);
        shared_data($) = 0;
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag.wait(0);
            
            int val = shared_data($);
            RL_ASSERT(val == 123);
        }
        else
        {
            shared_data($) = 123;
            flag.store(1, rl::memory_order_release);
            flag.notify_one();
        }
    }
};


struct test_atomic_racing_notifiers : rl::test_suite<test_atomic_racing_notifiers, 4>
{
    std::atomic<int> data;
    std::atomic<int> woken;

    void before()
    {
        data.store(0);
        woken.store(0);
    }

    void thread(unsigned index)
    {
        if (index == 0)
        {
            data.wait(0);
            woken.store(1);
        }
        else
        {
            // Three notifiers
            data.store(1);
            data.notify_one();
        }
    }
};

struct test_atomic_wait_race : rl::test_suite<test_atomic_wait_race, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
            RL_ASSERT(data.load() != 0);
        }
        else
        {
            data.store(1);
            data.notify_one();
        }
    }
};

struct test_atomic_forgot_notify : rl::test_suite<test_atomic_forgot_notify, 2, rl::test_result_deadlock>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
        }
        else
        {
            data.store(1);
        }
    }
};

struct test_atomic_notify_before_store : rl::test_suite<test_atomic_notify_before_store, 2, rl::test_result_deadlock>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
        }
        else
        {
            data.notify_one();
            data.store(1);
        }
    }
};

struct test_atomic_wait_wrong_value : rl::test_suite<test_atomic_wait_wrong_value, 2>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(1);
        }
        else
        {
            data.store(2);
            data.notify_one();
        }
    }
};

struct test_atomic_circular_wait : rl::test_suite<test_atomic_circular_wait, 2, rl::test_result_deadlock>
{
    std::atomic<int> data1;
    std::atomic<int> data2;

    void before()
    {
        data1.store(0);
        data2.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data1.wait(0);
            data2.store(1);
            data2.notify_one();
        }
        else
        {
            data2.wait(0);
            data1.store(1);
            data1.notify_one();
        }
    }
};

struct test_atomic_notify_all_insufficient : rl::test_suite<test_atomic_notify_all_insufficient, 3, rl::test_result_deadlock>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (index < 2)
        {
            data.wait(0);
            data.wait(1);
        }
        else
        {
            data.store(1);
            data.notify_all();
        }
    }
};

struct test_atomic_multiple_notify_one : rl::test_suite<test_atomic_multiple_notify_one, 3>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data($).wait(0);
            RL_ASSERT(data.load() == 1);
        }
        else
        {
            // Multiple notifiers
            data.store(1);
            data.notify_one();
        }
    }
};

struct test_atomic_double_wait : rl::test_suite<test_atomic_double_wait, 2, rl::test_result_deadlock>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data.wait(0);
            RL_ASSERT(data.load() == 1);
            data.wait(1);
        }
        else
        {
            data.store(2);
            data.store(1);
            data.notify_one();
        }
    }
};

struct test_atomic_notify_all_empty : rl::test_suite<test_atomic_notify_all_empty, 1>
{
    std::atomic<int> data;

    void before()
    {
        data.store(0);
    }

    void thread(unsigned index)
    {
        data.notify_all();
        data.store(1);
        data.notify_all();
    }
};

struct test_atomic_three_threads_in_chain : rl::test_suite<test_atomic_three_threads_in_chain, 3>
{
    std::atomic<int> flag1;
    std::atomic<int> flag2;
    std::atomic<int> flag3;

    void before()
    {
        flag1.store(0);
        flag2.store(0);
        flag3.store(0);
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag1.wait(0);
            flag2.store(1);
            flag2.notify_one();
        }
        else if (1 == index)
        {
            flag2.wait(0);
            flag3.store(1);
            flag3.notify_one();
        }
        else
        {
            // Start the chain
            flag1.store(1);
            flag1.notify_one();
            flag3.wait(0);
        }
    }
};

int main()
{
    rl::test_params params;
    params.iteration_count = 10000;

#define CHECK(x) if (!(x)) { std::cout << "Test failed at line " << __LINE__ << std::endl; return 1; }

    CHECK(rl::simulate<test_atomic_wait_notify_basic>(params));
    CHECK(rl::simulate<test_atomic_notify_one_insufficient>(params));
    CHECK(rl::simulate<test_atomic_notify_all>(params));
    CHECK(rl::simulate<test_atomic_wait_no_block>(params));
    CHECK(rl::simulate<test_atomic_wait_notify_cycles>(params));
    CHECK(rl::simulate<test_atomic_notify_no_waiters>(params));
    CHECK(rl::simulate<test_atomic_wait_memory_order>(params));
    CHECK(rl::simulate<test_atomic_racing_notifiers>(params));
    CHECK(rl::simulate<test_atomic_wait_race>(params));
    CHECK(rl::simulate<test_atomic_forgot_notify>(params));
    CHECK(rl::simulate<test_atomic_notify_before_store>(params));
    CHECK(rl::simulate<test_atomic_wait_wrong_value>(params));
    CHECK(rl::simulate<test_atomic_circular_wait>(params));
    CHECK(rl::simulate<test_atomic_notify_all_insufficient>(params));
    CHECK(rl::simulate<test_atomic_multiple_notify_one>(params));
    CHECK(rl::simulate<test_atomic_double_wait>(params));
    CHECK(rl::simulate<test_atomic_notify_all_empty>(params));
    CHECK(rl::simulate<test_atomic_three_threads_in_chain>(params));
    
    std::cout << "All atomic_flag tests passed!" << std::endl;

    return 0;
}

#else

int main() { return 0; }

#endif