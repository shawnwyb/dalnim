# dalnim

An animation engine for data structures and algorithms. An algorithm runs once and records what it did; the recording is then replayed at any point in time, so the whole run is scrubbable rather than only playable forwards.

## Quick start

Requires CMake 3.20+ and a C++20 compiler.
Tested with CMake 3.31 and Apple Clang 17 on macOS.

CMake downloads SDL3, Dear ImGui, and doctest on the first configure.

```bash
cmake -B build       # configure (once, or after editing CMakeLists.txt)
cmake --build build  # compile and link
./build/bin/dalnim   # run
```

All three at once, for a fresh clone:

```bash
cmake -B build && cmake --build build && ./build/bin/dalnim
```

Day to day, you only need build and run — CMake re-configures itself automatically when `CMakeLists.txt` changes:

```bash
cmake --build build && ./build/bin/dalnim
```

To start over, delete the build directory: `rm -rf build`

## Tests

Unit tests use [doctest](https://github.com/doctest/doctest), which CMake downloads on the first configure. Building does not run them:

```bash
cmake --build build                        # build the test binary
ctest --test-dir build --output-on-failure  # run it
```

`ctest` reports pass/fail per test binary. To see individual test cases, or to run one by name, invoke the binary directly:

```bash
./build/bin/dalnim_tests
./build/bin/dalnim_tests --test-case="the test harness runs"
```

Tests are built by default. To skip them, and skip the doctest download:

```bash
cmake -B build -DDALNIM_BUILD_TESTS=OFF
```