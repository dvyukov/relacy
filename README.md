[![Build Status](https://travis-ci.org/dvyukov/relacy.svg?branch=master)](https://travis-ci.org/dvyukov/relacy)

# Relacy Race Detector
Meticulous synchronization algorithm verifier for relaxed memory models

 * http://www.1024cores.net/home/relacy-race-detector
 * http://www.1024cores.net/home/relacy-race-detector/rrd-introduction

## Building - for contributors and maintainers of Relacy

To developer the Relacy library itself, use CMake to configure a build,
selecting your compiler and C++ standard of choice.

```
mkdir build && cd build
cmake \
  -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  ..
make -j 8
ctest # Run tests
```

To test that all supported toolchains on your platform build and run correctly,
use the top level Makefile as a helper to build N different configurations. For
example, on a Linux host, the following will create one CMake build directory
for GCC and Clang and for each C++ 11 through C++ 23. Then, it will run the
unit tests on all build configurations. This should be done before submitting
pull requests (at least until we can setup CI to run on pull requests).

```
make build-all -j 8
make test-all -j 8
```

## Toolchains known to work

At a minimum, a C++11 compiler is assumed. The below compilers have
been recently (Jan 2026) tested and confirmed to work

 - Linux x86_64
    - GCC-11-14, C++11 - C++23 (where GCC supports the version)
    - Clang-18-21, C++11 - C++23 (where Clang supports the version)
 - Mac x86_64 (Clang-15, 17), C++11 - C++23

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
