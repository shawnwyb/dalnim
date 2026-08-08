add_library(dalnim_core STATIC
    timeline.cpp
    algos/bubble_sort.cpp
    layout.cpp
)

target_include_directories(dalnim_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
