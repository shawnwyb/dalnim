#pragma once
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Unlike bubble sort, the pairs it swaps are not adjacent.
    EventLog selection_sort(std::vector<int> data);
}
