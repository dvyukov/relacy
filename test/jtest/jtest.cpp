#include "stdafx.h"

#include "../../relacy/relacy_java.hpp"





class stack
{
public:
    stack()
        : head_(0)
    {
    }

    void push(int data)
    {
        rl::var<node*> n = new node ();
        VAR(n)->VAR(data_) = data;
        node* next = head_.load(rl::memory_order_relaxed);
        for (;;)
        {
            VAR(n)->next_.store(next, rl::memory_order_relaxed);
            if (head_.compare_exchange_weak(next, VAR(n), rl::memory_order_release))
                break;
        }
    }

    int pop()
    {
        node* n = head_.load(rl::memory_order_acquire);
        for (;;)
        {
            if (0 == n)
                break;
            node* next = n->next_.load(rl::memory_order_relaxed);
            if (head_.compare_exchange_weak(n, next, rl::memory_order_acquire))
                break;
        }
        if (n)
        {
            int data = n->VAR(data_);
            return data;
        }
        else
        {
            return 0;
        }
    }

private:
    struct node
    {
        rl::atomic<node*> next_;
        rl::var<int> data_;
    };

    rl::atomic<node*> head_;

    stack(stack const&);
    stack& operator = (stack const&);
};




struct stack_test
{
    stack s_;

    int produced_count_;
    int consumed_count_;

    void before()
    {
        produced_count_ = 0;
        consumed_count_ = 0;
    }

    void after()
    {
        RL_ASSERT(4 == produced_count_);
        RL_ASSERT(4 == consumed_count_);
    }

    void invariant() { }

    void thread(unsigned /*index*/)
    {
        s_.push(rand() + 1);
        produced_count_ += 1;
        int data = s_.pop();
        RL_ASSERT(data);
        consumed_count_ += 1;
    }
};


struct test_api
{
    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned)
    {
        rl::jvolatile<int> jv1;
        rl::jvolatile<int> jv2 (2);
        rl::jvolatile<int> jv3 (jv2($));
        rl::jvolatile<int> jv4 (jv1);
        jv1($) = jv3($);
        jv1($) = 2;
        (int)jv1($);
        jv1($) += 1;
        jv1($) -= 1;
        int x = jv1($)++;
        x = jv1($)--;
        x = --jv1($);
        x = ++jv1($);

        rl::AtomicInteger ai, ai2(1), ai3(x), ai4(ai($)), ai5(ai);
        x = ai($).get();
        ai($).set(1);
        x = ai($).addAndGet(2);
        bool b = ai($).compareAndSet(1, 2);
        (void)b;
        x = ai($).addAndGet(2);
        x = ai($).getAndSet(2);
    }
};

struct test_seq_cst_volatiles
{
    rl::jvolatile<int> flag0;
    rl::jvolatile<int> flag1;
    rl::jvolatile<int> turn;

    rl::var<int> data;

    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            flag0($) = 1;
            turn($) = 1;
            while (flag1($) && 1 == turn($))
                rl::yield(1, $);
            data($) = 1;
            flag0($) = 0;
        }
        else
        {
            flag1($) = 1;
            turn($) = 0;
            while (flag0($) && 0 == turn($))
                rl::yield(1, $);
            data($) = 2;
            flag1($) = 0;
        }
    }
};

struct test_seq_cst_volatiles2
{
    rl::jvolatile<int> x;
    rl::jvolatile<int> y;

    int r1, r2, r3, r4;

    void before()
    {
        r1 = r2 = r3 = r4 = 0;
    }

    void invariant() { }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            x($) = 0;
        }
        else if (1 == index)
        {
            y($) = 0;
        }
        else if (2 == index)
        {
            r1 = x($);
            r2 = y($);
        }
        else if (3 == index)
        {
            r3 = y($);
            r4 = x($);
        }
    }

    void after()
    {
        RL_ASSERT(false == (r1 && !r2 && r3 && !r4));
    }
};

template<int expected>
struct test_unitialized_var
{
    rl::jvar<rl::jvar<int>*> www;

    void before() { }
    void after() { }
    void invariant() { }

    void thread(unsigned index)
    {
        if (0 == index)
        {
            www($) = new rl::jvar<int> (1);
        }
        else
        {
            while (0 == www($))
                rl::yield(1, $);
            int x = (*www($))($);
            RL_UNTIL(x == expected);
        }
    }
};

struct test_info_t
{
    rl::simulate_f      f;
    rl::thread_id_t     static_thread_count;
    rl::thread_id_t     dynamic_thread_count;
    rl::test_result_e   expected_result;
};

int main()
{
    test_info_t tests[] =
    {
        //!!! broken &rl::simulate<test_unitialized_var<0> >,
        { &rl::simulate<test_unitialized_var<1> >, 2, 0, rl::test_result_until_condition_hit },
        { &rl::simulate<test_seq_cst_volatiles>, 2, 0, rl::test_result_success },
        { &rl::simulate<test_seq_cst_volatiles2>, 4, 0, rl::test_result_success },
        { &rl::simulate<test_api>, 1, 0, rl::test_result_success },
        { &rl::simulate<stack_test>, 4, 0, rl::test_result_success }
    };

    for (size_t i = 0; i != sizeof(tests)/sizeof(*tests); ++i)
    {
        rl::ostringstream stream;
        rl::test_params params;
        params.iteration_count = 10000;
        params.output_stream = &stream;
        params.progress_stream = &stream;
        params.context_bound = 2;
        params.execution_depth_limit = 500;
        params.static_thread_count = tests[i].static_thread_count;
        params.dynamic_thread_count = tests[i].dynamic_thread_count;
        params.expected_result = tests[i].expected_result;

        if (false == tests[i].f(params))
        {
            std::cout << std::endl;
            std::cout << "FAILED" << std::endl;
            std::cout << stream.str();
            return 1;
        }
        else
        {
            std::cout << params.test_name << "...OK" << std::endl;
        }
    }

    std::cout << std::endl << "SUCCESS" << std::endl;
}

