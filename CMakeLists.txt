cmake_minimum_required(VERSION 3.20...3.31)
project(dalnim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emits build/compile_commands.json for clangd and IDE tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect every executable in one place, whatever subdirectory it was declared in.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(DALNIM_BUILD_TESTS "Build the unit tests" ON)

add_subdirectory(src)

if(DALNIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
