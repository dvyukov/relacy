#include "../relacy/relacy_std.hpp"

#if __cplusplus >= 202002L
struct atomic_init_test : rl::test_suite<atomic_init_test, 1>
{
    void thread(unsigned index)
    {
        rl::atomic<int> i;
        i($).load();
    }
};
#else
struct atomic_init_test : rl::test_suite<atomic_init_test, 1, rl::test_result_unitialized_access>
{
    void thread(unsigned index)
    {
        rl::atomic<int> i;
        i($).load();
    }
};
#endif

int main()
{
    rl::test_params p;
    p.iteration_count = 100;
    return rl::simulate<atomic_init_test>();
}
