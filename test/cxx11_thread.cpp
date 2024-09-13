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
            // Verify move ctor and move assignment.
            std::thread a{[] {}, $};
            RL_ASSERT(std::this_thread::get_id() != a.get_id());
            std::thread b = std::move(a);
            RL_ASSERT(std::this_thread::get_id() != b.get_id());
            a = std::move(b);
            RL_ASSERT(std::this_thread::get_id() != a.get_id());
        }
    }
};

int main()
{
    rl::test_params p;
    p.iteration_count = 1000;
    rl::simulate<cxx11_thread_tests>(p);
    rl::simulate<cxx11_thread_basic_ops_test>(p);
    return 0;
}
