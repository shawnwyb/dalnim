#pragma once
#include <cstddef>
#include <optional>
#include "core/grid_layout.hpp"

namespace dalnim {
    // Draws the grid at the moment `t`, ringing the cell the run started from.
    // Returns the cell the mouse just clicked, so the caller can move the start there.
    std::optional<std::size_t> draw_grid(const GridAnimation& anim, double t, std::size_t start);
}
