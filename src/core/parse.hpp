#pragma once
#include <string_view>
#include <vector>
#include "core/dp_table.hpp"
#include "core/graph.hpp"
#include "core/grid.hpp"
#include "core/intervals.hpp"
#include "core/tree.hpp"

namespace dalnim {
    // Pulls every integer out of `text`, ignoring whatever separates them.
    // Anything that is not a well-formed int is skipped rather than reported.
    std::vector<int> parse_int_list(std::string_view text);

    // One row per line, blank lines skipped. Short rows are padded with zeroes so
    // the result is always rectangular.
    Grid parse_grid(std::string_view text);

    // Values in slot order, so the second and third entries are the root's children.
    // A token that is not a number, such as "." or "null", means the slot is empty.
    Tree parse_tree(std::string_view text);

    // One line per node, listing the nodes it joins. A line with no numbers means a
    // node with no edges. Every edge is recorded at both ends, so order never matters.
    Graph parse_graph(std::string_view text);

    // Two lines, the first word across the table and the second down it.
    DpTable parse_dp_table(std::string_view text);

    // One interval per line, written as a start and an end. A line holding fewer
    // than two numbers is dropped, and an end before its start is turned around,
    // so a half-typed line cannot produce a backwards bar.
    Intervals parse_intervals(std::string_view text);
}
