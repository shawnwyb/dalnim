add_library(dalnim_core STATIC
    timeline.cpp
    algos/bubble_sort.cpp
    algos/selection_sort.cpp
    algos/registry.cpp
    algos/flood_fill.cpp
    algos/bfs.cpp
    algos/monotonic_stack.cpp
    stack_layout.cpp
    replay.cpp
    tree.cpp
    tree_layout.cpp
    algos/tree_dfs.cpp
    algos/cycle_detect.cpp
    linked_list.cpp
    graph.cpp
    dp_table.cpp
    dp_layout.cpp
    algos/edit_distance.cpp
    graph_layout.cpp
    algos/graph_bfs.cpp
    list_layout.cpp
    grid.cpp
    event.cpp
    grid_layout.cpp
    array_layout.cpp
    parse.cpp
    ease.cpp
)

target_include_directories(dalnim_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
