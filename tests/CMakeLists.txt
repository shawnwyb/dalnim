include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.5.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(doctest)

add_executable(dalnim_tests
    test_main.cpp
    test_timeline.cpp
)
target_link_libraries(dalnim_tests PRIVATE doctest::doctest dalnim_core)

add_test(NAME dalnim_tests COMMAND dalnim_tests)
