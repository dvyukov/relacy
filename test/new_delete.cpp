#include "../relacy/relacy_std.hpp"
#include "../relacy/relacy_cli.hpp"

#include <vector>

struct new_delete_tests : rl::test_suite<new_delete_tests, 1>
{
    void thread(unsigned index)
    {
        std::vector<int> vs;
        vs.push_back(1);
        vs.push_back(2);
        vs.push_back(3);
    }
};

int main()
{
    rl::test_params p;
    p.iteration_count = 10;
    rl::simulate<new_delete_tests>(p);
    return 0;
}
