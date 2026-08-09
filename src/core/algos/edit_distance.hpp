#pragma once
#include "core/dp_table.hpp"
#include "core/event.hpp"

namespace dalnim {
    // How many single-character edits turn one word into the other. Each cell is
    // filled from the cells above and to its left, and the Compare says which one
    // it took its answer from.
    EventLog edit_distance(DpTable table);
}
