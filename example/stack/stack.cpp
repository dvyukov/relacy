#include "stdafx.h"

#include "../../relacy/relacy_std.hpp"


// TEST FAILS WITH "ACCESS TO FREED MEMORY"

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
        n($)->data_($) = data;
        node* next = head_.load(rl::memory_order_relaxed);
        for (;;)
        {
            n($)->next_.store(next, rl::memory_order_relaxed);
            if (head_.compare_exchange_weak(next, n($), rl::memory_order_release))
                break;
        }
    }

    int pop()
    {
        node* n = head_.load(rl::memory_order_relaxed);
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
            int data = n->data_($);
            delete n;
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
        std::atomic<node*> next_;
        rl::var<int> data_;
    };

    std::atomic<node*> head_;

    stack(stack const&);
    stack& operator = (stack const&);
};

int count_threads = 0;

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
        RL_ASSERT(count_threads == produced_count_);
        RL_ASSERT(count_threads == consumed_count_);
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




int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        return EINVAL;
    }

    try
    {
        count_threads = std::atoi(argv[1]);
        if (count_threads <= 0)
        {
            std::cerr << "The number of threads must be a positive number" << std::endl;
            return EINVAL;
        }

        rl::test_params p;
        p.static_thread_count = count_threads;
        rl::simulate<stack_test>(p);
    }
    catch(std::exception const & e)
    {
        (void)e;
        std::cerr << "Wrong argument. The number of threads must be a number" << std::endl;
        return EINVAL;
    }

    return 0;
}

