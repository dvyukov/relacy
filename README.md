[![Build Status](https://travis-ci.org/dvyukov/relacy.svg?branch=master)](https://travis-ci.org/dvyukov/relacy)

# Relacy Race Detector
Meticulous synchronization algorithm verifier for relaxed memory models

 * http://www.1024cores.net/home/relacy-race-detector
 * http://www.1024cores.net/home/relacy-race-detector/rrd-introduction

## Compilers known to work

At a minimum, a C++11 compiler is assumed. The below compilers have
been recently (Aug 2024) tested and confirmed to work

 - Linux x86_64
    - GCC-11-14, C++11-20
    - Clang-18-21
 - Mac x86_64 (Clang-15)

## Overriding `std::` types

Relacy defines its own version of types like `rl::atomic`, `rl::thread`, etc,
to allow writing Relacy tests in terms of the `rl::` types. However, in some
cases it can be convenient to test existing code that uses `std::` names
without rewriting the code to use `rl::` types.

Relacy has support to intercept standard headers like `<atomic>` and alias the
std types in the rl namespace. To do this, you must

 - Add `-Irelacy/fakestd` to your include path
 - Ensure "relacy/relacy_std.hpp" is the first include in your .cpp source file

Note: This approach may not work as compilers and standard library implementations
evolve, and may require changes to Relacy for the fakestd implementation to
keep working.
