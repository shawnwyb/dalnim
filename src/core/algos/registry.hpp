#pragma once
#include <cstddef>
#include <span>
#include <variant>
#include <vector>
#include "core/event.hpp"
#include "core/grid.hpp"

namespace dalnim {
    using ArrayAlgorithm = EventLog (*)(std::vector<int>);
    using GridAlgorithm = EventLog (*)(Grid, std::size_t start);

    // Which shape of input an algorithm wants is part of the algorithm, not a
    // separate flag, so the two can never disagree.
    struct Algorithm {
        const char* name;
        std::variant<ArrayAlgorithm, GridAlgorithm> run;
    };

    bool wants_array(const Algorithm& algo);

    // Everything the app can offer, in menu order. Adding one is a line in registry.cpp.
    std::span<const Algorithm> algorithms();
}
