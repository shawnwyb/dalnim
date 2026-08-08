#pragma once
#include <cstddef>
#include "core/event.hpp"
#include "core/grid.hpp"

namespace dalnim {
    // Repaints the region of same-valued cells reachable from `start` by
    // up/down/left/right steps. Emits nothing if there is no work to do.
    EventLog flood_fill(Grid grid, std::size_t start, int fill_value);
}
