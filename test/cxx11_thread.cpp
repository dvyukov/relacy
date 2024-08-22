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

int main()
{
    rl::test_params p;
    p.iteration_count = 1000;
    rl::simulate<cxx11_thread_tests>(p);
    return 0;
}
