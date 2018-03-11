#pragma once

#include "../relacy/pthread.h"



struct test_futex
{
	rl::atomic<int> state;
	int wakeres;
	int waitres;
	
	void before()
	{
		state.store(0, rl::memory_order_relaxed);
		wakeres = 0;
		waitres = 0;
	}
	
	void after()
	{
		assert((waitres == 0 && wakeres == 1)
					 || (waitres == EWOULDBLOCK && wakeres == 0)
					 || (waitres == EINTR && wakeres == 0));
	}

    void invariant() { }
	
	void thread(unsigned index)
	{
		if (index == 0)
		{
			state.store(1, std::memory_order_relaxed);
			wakeres = futex(&state, FUTEX_WAKE, 1, 0, 0, 0);
		}
		else
		{
			waitres = EINTR;
			while (state.load(rl::memory_order_relaxed) == 0)
			{
				waitres = futex(&state, FUTEX_WAIT, 0, 0, 0, 0);
			}
		}
	}
};




struct test_futex_deadlock
{
	rl::atomic<int> state;

    void before() { }
    void after() { }
    void invariant() { }
	
	void thread(unsigned index)
	{
    (void)index;
		state.store(0, rl::memory_order_relaxed);
		int rv = futex(&state, FUTEX_WAIT, 0, 0, 0, 0);
		assert(rv == EINTR);
	}
};




struct test_futex_sync1
{
	rl::atomic<int> state;
	VAR_T(int) data;
	
	void before()
	{
		state.store(0, rl::memory_order_relaxed);
		VAR(data) = 0;
	}

    void after() { }
    void invariant() { }
	
	void thread(unsigned index)
	{
		if (index == 0)
		{
			VAR(data) = 1;
			state.store(1, std::memory_order_release);
			futex(&state, FUTEX_WAKE, 1, 0, 0, 0);
		}
		else
		{
			int rv = futex(&state, FUTEX_WAIT, 0, 0, 0, 0);
			assert(rv == 0 || rv == EWOULDBLOCK || rv == EINTR);
			if (rv == 0)
			{
				assert(VAR(data) == 1);
				assert(state.load(rl::memory_order_relaxed) == 1);
				RL_UNTIL(true);
			}
		}
	}
};




struct test_futex_sync2
{
	rl::atomic<int> state;
	VAR_T(int) data;
	
	void before()
	{
		state.store(0, rl::memory_order_relaxed);
		VAR(data) = 0;
	}

    void after() { }
    void invariant() { }
	
	void thread(unsigned index)
	{
		if (index == 0)
		{
			VAR(data) = 1;
			state.store(1, std::memory_order_release);
			futex(&state, FUTEX_WAKE, 1, 0, 0, 0);
		}
		else
		{
			int rv = futex(&state, FUTEX_WAIT, 0, 0, 0, 0);
			assert(rv == 0 || rv == EWOULDBLOCK || rv == EINTR);
			if (rv == EWOULDBLOCK)
			{
				assert(VAR(data) == 1);
				assert(state.load(rl::memory_order_relaxed) == 1);
				RL_UNTIL(true);
			}
		}
	}
};




struct test_futex_intr
{
	rl::atomic<int> state;
	VAR_T(int) data;
	
	void before()
	{
		state.store(0, rl::memory_order_relaxed);
		VAR(data) = 0;
	}

    void after() { }
    void invariant() { }
	
	void thread(unsigned index)
	{
		if (index == 0)
		{
			VAR(data) = 1;
			state.store(1, std::memory_order_release);
			futex(&state, FUTEX_WAKE, 1, 0, 0, 0);
		}
		else
		{
			int rv = futex(&state, FUTEX_WAIT, 0, 0, 0, 0);
			assert(rv == 0 || rv == EWOULDBLOCK || rv == EINTR);
			RL_UNTIL(rv == EINTR);
		}
	}
};

