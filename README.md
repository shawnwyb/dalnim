# dalnim

## Quick start

Requires CMake 3.20+ and a C++20 compiler.
Tested with CMake 3.31 and Apple Clang 17 on macOS.

```bash
cmake -B build       # configure (once, or after editing CMakeLists.txt)
cmake --build build  # compile and link
./build/dalnim       # run
```

All three at once, for a fresh clone:

```bash
cmake -B build && cmake --build build && ./build/dalnim
```

Day to day, you only need build and run — CMake re-configures itself
automatically when `CMakeLists.txt` changes:

```bash
cmake --build build && ./build/dalnim
```

To start over, delete the build directory: `rm -rf build`
