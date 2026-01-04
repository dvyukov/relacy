#ifndef RL_BITS_ATOMIC_BASE
#define RL_BITS_ATOMIC_BASE

// For C

enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
};

#endif
