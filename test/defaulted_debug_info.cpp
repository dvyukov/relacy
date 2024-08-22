#if __cplusplus >= 202002L

#include "../relacy/relacy_std.hpp"

struct defaulted_debug_info_test : rl::test_suite<defaulted_debug_info_test, 1>
{
    void thread(unsigned index)
    {
        // Verify that calling member methods without passing the `$` argument
        // explicitly compiles C++20 mode. In C++20 mode, the debug_info_param
        // is defaulted with std::source_location.

        rl::mutex mtx;
        rl::condition_variable cv;
        rl::atomic<int> i;

        i.store(0);
        int expected = 0;
        i.compare_exchange_strong(expected, 1);
        i.exchange(1);

        if (i.load() == 100) {
            mtx.lock();
            cv.wait(mtx);
            mtx.unlock();
        } else {
            cv.notify_all();
            cv.notify_one();
        }
    }
};

int main()
{
    rl::test_params p;
    p.iteration_count = 1000;
    rl::simulate<defaulted_debug_info_test>();
    return 0;
}

#else
int main() { return 0; }
#endif
