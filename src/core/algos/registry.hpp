#pragma once
#include <cstddef>
#include <span>
#include <variant>
#include <vector>
#include "core/event.hpp"
#include "core/grid.hpp"
#include "core/intervals.hpp"
#include "core/dp_table.hpp"
#include "core/graph.hpp"
#include "core/linked_list.hpp"
#include "core/tree.hpp"

namespace dalnim {
    using ArrayAlgorithm = EventLog (*)(std::vector<int>);
    using GridAlgorithm = EventLog (*)(Grid, std::size_t start);
    using TreeAlgorithm = EventLog (*)(Tree);
    using ListAlgorithm = EventLog (*)(LinkedList);
    using GraphAlgorithm = EventLog (*)(Graph, std::size_t start);
    using DpAlgorithm = EventLog (*)(DpTable);
    using IntervalAlgorithm = EventLog (*)(Intervals);

    // How a run should be pictured. Separate from the input shape, because two
    // algorithms can read the same input and still want different pictures.
    enum class View { Bars, Grid, Stack, Set, Tree, List, Graph, Dp, Intervals };

    // What the algorithm is about, which is what someone browsing the menu is
    // looking for. Deliberately not derived from View: flood fill is drawn on a
    // grid but is a graph problem, and the heading has to say so.
    enum class Topic { Sorting, ArraysAndHashing, Stack, LinkedList, Trees, Graphs, Dp, Intervals };

    const char* topic_name(Topic topic);

    struct Algorithm {
        const char* name;
        Topic topic;
        View view;
        std::variant<ArrayAlgorithm, GridAlgorithm, TreeAlgorithm, ListAlgorithm,
                     GraphAlgorithm, DpAlgorithm, IntervalAlgorithm> run;
    };

    // Everything the app can offer, in menu order. Adding one is a line in registry.cpp.
    // Entries sharing a topic sit together, so the menu can head each run with its
    // topic in a single pass.
    std::span<const Algorithm> algorithms();
}
