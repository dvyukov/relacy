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

struct atomic_use_after_scope_detected : rl::test_suite<atomic_use_after_scope_detected, 1, rl::test_result_access_to_freed_memory>
{
  static size_t const dynamic_thread_count = 1;

  void thread(unsigned) {
      std::atomic<int>* ip;
      {
          std::atomic<int> i{0};
          ip = &i;
      }
      (*ip)($).store(5);
  }
};

int main()
{
    rl::test_params p;
    p.iteration_count = 100;

#define CHECK(x) if (!(x)) { std::cout << "Test failed at line " << __LINE__ << std::endl; return 1; }

    CHECK(rl::simulate<atomic_init_test>());
    CHECK(rl::simulate<atomic_use_after_scope_detected>());
    std::cout << "All atomic_flag tests passed!" << std::endl;
    return 0;
}
