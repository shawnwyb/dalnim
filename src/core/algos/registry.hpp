#pragma once
#include <cstddef>
#include <span>
#include <variant>
#include <vector>
#include "core/event.hpp"
#include "core/grid.hpp"
#include "core/linked_list.hpp"
#include "core/tree.hpp"

namespace dalnim {
    using ArrayAlgorithm = EventLog (*)(std::vector<int>);
    using GridAlgorithm = EventLog (*)(Grid, std::size_t start);
    using TreeAlgorithm = EventLog (*)(Tree);
    using ListAlgorithm = EventLog (*)(LinkedList);

    // How a run should be pictured. Separate from the input shape, because two
    // algorithms can read the same input and still want different pictures.
    enum class View { Bars, Grid, Stack, Tree, List };

    struct Algorithm {
        const char* name;
        View view;
        std::variant<ArrayAlgorithm, GridAlgorithm, TreeAlgorithm, ListAlgorithm> run;
    };

    // Everything the app can offer, in menu order. Adding one is a line in registry.cpp.
    std::span<const Algorithm> algorithms();
}
