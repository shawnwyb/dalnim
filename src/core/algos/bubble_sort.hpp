#pragma once
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Sorts a copy of data and reports what it did. The caller keeps the original,
    // which is the starting state every event in the log is relative to.
    EventLog bubble_sort(std::vector<int> data);
}
