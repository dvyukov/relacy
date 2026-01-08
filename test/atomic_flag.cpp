#include "../relacy/relacy_std.hpp"

struct test_atomic_flag_race_set : rl::test_suite<test_atomic_flag_race_set, 3>
{
    rl::atomic_flag flag = RL_ATOMIC_FLAG_INIT;
    std::atomic<int> first_count{0};

    void thread(unsigned index)
    {
        bool was_first = !flag.test_and_set(rl::memory_order_seq_cst, $);
        if (was_first)
        {
            first_count($).fetch_add(1, rl::memory_order_relaxed);
        }
    }

    void after()
    {
        RL_ASSERT(first_count($).load() == 1);
    }
};

struct test_atomic_flag_spinlock : rl::test_suite<test_atomic_flag_spinlock, 3>
{
    rl::atomic_flag lock = RL_ATOMIC_FLAG_INIT;
    rl::var<int> counter{0};

    void thread(unsigned index)
    {
        while (lock.test_and_set(rl::memory_order_acquire, $));

        int val = counter($);
        counter($) = val + 1;

        lock.clear(rl::memory_order_release, $);
    }

    void after()
    {
        RL_ASSERT(counter($) == 3);
    }
};

#if __cplusplus >= 202002L
struct test_atomic_flag_memory_order : rl::test_suite<test_atomic_flag_memory_order, 2>
{
    rl::atomic_flag flag = RL_ATOMIC_FLAG_INIT;
    rl::var<int> data{0};

    void thread(unsigned index)
    {
        if (0 == index)
        {
            data($) = 42;
            flag.test_and_set(rl::memory_order_release);
        }
        else
        {
            while (!flag.test(rl::memory_order_acquire));
            int val = data($);
            RL_ASSERT(val == 42);
        }
    }
};

struct test_atomic_flag_wait_notify : rl::test_suite<test_atomic_flag_wait_notify, 2>
{
    rl::atomic_flag flag = RL_ATOMIC_FLAG_INIT;

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag.wait(false);
            RL_ASSERT(flag.test());
        }
        else
        {
            flag.test_and_set();
            flag.notify_one();
        }
    }
};

struct test_atomic_wait_on_new_value : rl::test_suite<test_atomic_wait_on_new_value, 2, rl::test_result_deadlock>
{
    rl::atomic_flag flag = RL_ATOMIC_FLAG_INIT;

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag.wait(true);
        }
        else
        {
            flag.test_and_set();
            flag.notify_one();
        }
    }
};

struct test_atomic_flag_notify_before_set : rl::test_suite<test_atomic_flag_notify_before_set, 2, rl::test_result_deadlock>
{
    rl::atomic_flag flag = RL_ATOMIC_FLAG_INIT;

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag.wait(true);
        }
        else
        {
            flag.notify_one();
            flag.test_and_set();
        }
    }
};

struct test_atomic_flag_circular_wait : rl::test_suite<test_atomic_flag_circular_wait, 2, rl::test_result_deadlock>
{
    rl::atomic_flag flag1 = RL_ATOMIC_FLAG_INIT;
    rl::atomic_flag flag2 = RL_ATOMIC_FLAG_INIT;

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag1.wait(true);
            flag2.test_and_set();
            flag2.notify_one();
        }
        else
        {
            flag2.wait(true);
            flag1.test_and_set();
            flag1.notify_one();
        }
    }
};

#endif

int main()
{
    rl::test_params params;
    params.iteration_count = 10000;

#define CHECK(x) if (!(x)) { std::cout << "Test failed at line " << __LINE__ << std::endl; return 1; }

    CHECK(rl::simulate<test_atomic_flag_race_set>(params));
    CHECK(rl::simulate<test_atomic_flag_spinlock>(params));
#if __cplusplus >= 202002L
    CHECK(rl::simulate<test_atomic_flag_memory_order>(params));
    CHECK(rl::simulate<test_atomic_flag_wait_notify>(params));
    CHECK(rl::simulate<test_atomic_flag_notify_before_set>(params));
#endif
    std::cout << "All atomic_flag tests passed!" << std::endl;
    return 0;
}
