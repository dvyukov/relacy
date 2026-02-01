#include "../relacy/relacy_std.hpp"
#include <mutex>

struct unique_lock_basic : rl::test_suite<unique_lock_basic, 4> {
    std::mutex mtx;
    int counter = 0;

    void thread(unsigned tid) {
        std::unique_lock<std::mutex> lock(mtx, $);
        ++counter;
    }

    void after() {
        RL_ASSERT(counter == 4);
    }
};

#ifdef __cpp_lib_scoped_lock

struct scoped_lock_basic : rl::test_suite<scoped_lock_basic, 4> {
    std::mutex mtx;
    int counter = 0;

    void thread(unsigned tid) {
        std::scoped_lock<std::mutex> lock(mtx);
        ++counter;
    }

    void after() {
        RL_ASSERT(counter == 4);
    }
};

#endif // __cpp_lib_scoped_lock

int main()
{
    rl::test_params p;
    p.iteration_count = 100;

#define CHECK(x) if (!(x)) { std::cout << "Test failed at line " << __LINE__ << std::endl; return 1; }

    CHECK(rl::simulate<unique_lock_basic>());
#ifdef __cpp_lib_scoped_lock
    CHECK(rl::simulate<scoped_lock_basic>());
#endif // __cpp_lib_scoped_lock
    std::cout << "All stdmutex.cpp tests passed!" << std::endl;
    return 0;
}
