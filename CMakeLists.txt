cmake_minimum_required(VERSION 3.20...3.31)
project(dalnim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emits build/compile_commands.json for clangd and IDE tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(dalnim src/main.cpp)
