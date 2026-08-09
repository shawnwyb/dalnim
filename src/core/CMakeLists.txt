add_library(dalnim_core STATIC
    timeline.cpp
    algos/bubble_sort.cpp
    algos/selection_sort.cpp
    algos/registry.cpp
    algos/flood_fill.cpp
    grid.cpp
    grid_layout.cpp
    array_layout.cpp
    parse.cpp
    ease.cpp
)

target_include_directories(dalnim_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
