#include "core/algos/registry.hpp"
#include <utility>
#include "core/algos/bfs.hpp"
#include "core/algos/monotonic_stack.hpp"
#include "core/algos/cycle_detect.hpp"
#include "core/algos/graph_bfs.hpp"
#include "core/algos/tree_dfs.hpp"
#include "core/algos/bubble_sort.hpp"
#include "core/algos/flood_fill.hpp"
#include "core/algos/selection_sort.hpp"

namespace dalnim {
namespace {
    EventLog flood_fill_with_one(Grid grid, std::size_t start) {
        return flood_fill(std::move(grid), start, 1);
    }

    constexpr Algorithm kAll[] = {
        {"bubble sort", View::Bars, ArrayAlgorithm{&bubble_sort}},
        {"selection sort", View::Bars, ArrayAlgorithm{&selection_sort}},
        {"next greater element", View::Stack, ArrayAlgorithm{&next_greater}},
        {"flood fill", View::Grid, GridAlgorithm{&flood_fill_with_one}},
        {"breadth-first search", View::Grid, GridAlgorithm{&bfs}},
        {"tree depth-first walk", View::Tree, TreeAlgorithm{&tree_dfs}},
        {"cycle detection", View::List, ListAlgorithm{&detect_cycle}},
        {"graph breadth-first search", View::Graph, GraphAlgorithm{&graph_bfs}},
    };
}

std::span<const Algorithm> algorithms() {
    return kAll;
}
}
