#pragma once
#include <cstddef>
#include "core/event.hpp"
#include "core/grid.hpp"

namespace dalnim {
    // Breadth-first flood from `start` across cells holding zero; anything else is
    // a wall. Cells are marked frontier when queued and visited when taken off.
    EventLog bfs(Grid grid, std::size_t start);
}
