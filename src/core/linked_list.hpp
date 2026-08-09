#pragma once
#include <cstddef>
#include <optional>
#include <vector>

namespace dalnim {
    // Nodes in a row, each linking to the next. The last one links wherever
    // `cycle_to` says, or nowhere at all, which is how a loop is described.
    struct LinkedList {
        std::vector<int> values;
        std::optional<std::size_t> cycle_to;
    };

    std::optional<std::size_t> next_of(const LinkedList& list, std::size_t node);
    bool has_cycle(const LinkedList& list);
}
