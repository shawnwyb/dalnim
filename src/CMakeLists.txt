add_subdirectory(core)

add_executable(dalnim main.cpp)
target_link_libraries(dalnim PRIVATE dalnim_core)
