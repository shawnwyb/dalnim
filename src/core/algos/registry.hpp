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

    // How a run should be pictured. Separate from the input shape, because two
    // algorithms can read the same input and still want different pictures.
    enum class View { Bars, Grid, Stack };

    struct Algorithm {
        const char* name;
        View view;
        std::variant<ArrayAlgorithm, GridAlgorithm> run;
    };

    bool wants_array(const Algorithm& algo);

    // Everything the app can offer, in menu order. Adding one is a line in registry.cpp.
    std::span<const Algorithm> algorithms();
}
