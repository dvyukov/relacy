#pragma once

#include "../relacy/relacy_std.hpp"



struct test_condvar : rl::test_suite<test_condvar, 2>
{
    std::mutex mtx;
    std::condition_variable cv;
    rl::var<int> data;

    void before()
    {
        data($) = 0;
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            mtx.lock($);
            data($) += 1;
            mtx.unlock($);
            cv.notify_one($);
        }
        else
        {
            mtx.lock($);
            while (0 == data($))
            {
                cv.wait(mtx, $);
            }
            mtx.unlock($);
        }
    }
};

struct test_condvar_wait_pred : rl::test_suite<test_condvar_wait_pred, 2>
{
    std::mutex mtx;
    std::condition_variable cv;
    rl::var<int> data;

    void before()
    {
        data($) = 0;
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            mtx.lock($);
            data($) += 1;
            mtx.unlock($);
            cv.notify_one($);
        }
        else
        {
            std::unique_lock lock(mtx, $);
            cv.wait(mtx, [&] { return 0 != data($); }, $);
        }
    }
};




struct test_condvar2 : rl::test_suite<test_condvar2, 3>
{
    rl::var<int> stage;
    std::mutex mtx;
    std::condition_variable cv;

    void before()
    {
        stage($) = 0;
    }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            mtx.lock($);
            stage($) += 1;
            cv.notify_all($);
            while (stage($) != 2)
                cv.wait(mtx, $);
            mtx.unlock($);
        }
        else if (1 == index)
        {
            mtx.lock($);
            while (stage($) != 1)
                cv.wait(mtx, $);
            stage($) += 1;
            cv.notify_all($);
            mtx.unlock($);
        }
        else if (2 == index)
        {
            mtx.lock($);
            while (stage($) != 2)
                cv.wait(mtx, $);
            mtx.unlock($);
        }
    }
};

