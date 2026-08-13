#include "core/algos/registry.hpp"
#include <utility>
#include "core/algos/bfs.hpp"
#include "core/algos/monotonic_stack.hpp"
#include "core/algos/cycle_detect.hpp"
#include "core/algos/edit_distance.hpp"
#include "core/algos/graph_bfs.hpp"
#include "core/algos/has_duplicate.hpp"
#include "core/algos/meeting_rooms.hpp"
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
        {"bubble sort", Topic::Sorting, View::Bars, ArrayAlgorithm{&bubble_sort}},
        {"selection sort", Topic::Sorting, View::Bars, ArrayAlgorithm{&selection_sort}},
        {"contains duplicate", Topic::ArraysAndHashing, View::Set, ArrayAlgorithm{&has_duplicate}},
        {"next greater element", Topic::Stack, View::Stack, ArrayAlgorithm{&next_greater}},
        {"cycle detection", Topic::LinkedList, View::List, ListAlgorithm{&detect_cycle}},
        {"tree depth-first walk", Topic::Trees, View::Tree, TreeAlgorithm{&tree_dfs}},
        {"flood fill", Topic::Graphs, View::Grid, GridAlgorithm{&flood_fill_with_one}},
        {"breadth-first search", Topic::Graphs, View::Grid, GridAlgorithm{&bfs}},
        {"graph breadth-first search", Topic::Graphs, View::Graph, GraphAlgorithm{&graph_bfs}},
        {"edit distance", Topic::Dp, View::Dp, DpAlgorithm{&edit_distance}},
        {"meeting rooms", Topic::Intervals, View::Intervals, IntervalAlgorithm{&can_attend_all}},
    };
}

const char* topic_name(Topic topic) {
    switch (topic) {
        case Topic::Sorting:          return "sorting";
        case Topic::ArraysAndHashing: return "arrays & hashing";
        case Topic::Stack:            return "stack";
        case Topic::LinkedList:       return "linked list";
        case Topic::Trees:            return "trees";
        case Topic::Graphs:           return "graphs";
        case Topic::Dp:               return "dynamic programming";
        case Topic::Intervals:        return "intervals";
    }
    return "";
}

std::span<const Algorithm> algorithms() {
    return kAll;
}
}
