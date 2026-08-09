#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include "core/event.hpp"
#include "core/grid.hpp"

namespace dalnim {
    // Grid cells never move, so unlike an array there is nothing to interpolate.
    // The log is kept as recorded and replayed up to whatever moment is asked for.
    struct GridAnimation {
        Grid initial;
        EventLog log;
        double duration = 0.0;
    };

    GridAnimation build_grid_animation(Grid initial, EventLog log);

    std::vector<int> grid_values_at(const GridAnimation& anim, double t);

    std::optional<std::size_t> highlighted_at(const GridAnimation& anim, double t);
}
