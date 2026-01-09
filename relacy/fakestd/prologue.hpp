#ifndef RL_RELACY_PROLOGUE_HPP
#define RL_RELACY_PROLOGUE_HPP

#if defined(__GNUC__) || defined(__clang__)

// Do not include bits/shared_ptr_atomic.h from libstdc++++
// It defines its own atomic implementation that conflicts with relacy
#define _SHARED_PTR_ATOMIC_H

#endif

#endif
