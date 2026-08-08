add_library(dalnim_core STATIC
    timeline.cpp
    algos/bubble_sort.cpp
    algos/selection_sort.cpp
    algos/registry.cpp
    layout.cpp
    parse.cpp
    ease.cpp
)

target_include_directories(dalnim_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
