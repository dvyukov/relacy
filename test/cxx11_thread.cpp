#include "../../relacy/relacy_std.hpp"
#include "../../relacy/relacy_cli.hpp"

#include <thread>

struct cxx11_thread_tests : rl::test_suite<cxx11_thread_tests, 1>
{
    static size_t const dynamic_thread_count = 1;

    void thread(unsigned index)
    {
        if (index == 0) {
            rl::atomic<int> i{0};
            i($).store(0);

            std::thread t{[&] {
                ++i($);
            }, $};
            ++i($);

            t.join();
            RL_ASSERT(i($).load() == 2);
        }
    }
};

struct cxx11_thread_basic_ops_test : rl::test_suite<cxx11_thread_basic_ops_test, 1>
{
    static size_t const dynamic_thread_count = 1;

    void thread(unsigned index)
    {
        {
            // Verify we can create an empty thread and safely destroy it
            std::thread t;
        }

        {
            // Verify move ctor and move assignment, and comparison ops.
            std::thread a{[] {}, $};
            RL_ASSERT(std::this_thread::get_id() != a.get_id());
            std::size_t hash1 = std::hash<std::thread::id>{}(a.get_id());
            RL_ASSERT(hash1 != std::hash<std::thread::id>{}(std::this_thread::get_id()));

            std::thread b = std::move(a);
            RL_ASSERT(std::this_thread::get_id() != b.get_id());
            std::size_t hash2 = std::hash<std::thread::id>{}(b.get_id());
            RL_ASSERT(hash1 == hash2);

            a = std::move(b);
            RL_ASSERT(std::this_thread::get_id() != a.get_id());
        }

        {
            // Verify get_id works on an empty thread.
            std::thread t;
            RL_ASSERT(t.get_id() == std::thread::id{});
        }
    }
};

#if __cplusplus >= 202002L
struct thread_variadic_args : rl::test_suite<thread_variadic_args, 1>
{
    static size_t const dynamic_thread_count = 2;

    void thread(unsigned index)
    {
        if (index == 0) {
            std::atomic<int> i{0};

            std::thread t{[&](int a) {
                i.fetch_add(a);
            }, 10};
            i.fetch_add(1);
            t.join();

            t = std::thread{[&](int a, int b) {
                i.fetch_add(a);
                i.fetch_add(b);
            }, 20, 30};
            i.fetch_add(1);
            t.join();

            RL_ASSERT(i($).load() == 62);
        }
    }
};
#endif

int main()
{
#define CHECK(x) if (!(x)) { std::cout << "Test failed at line " << __LINE__ << std::endl; return 1; }

    rl::test_params p;
    p.iteration_count = 1000;
    CHECK(rl::simulate<cxx11_thread_tests>(p));
    CHECK(rl::simulate<cxx11_thread_basic_ops_test>(p));
#if __cplusplus >= 202002L
    CHECK(rl::simulate<thread_variadic_args>(p));
#endif
    return 0;
}
