#pragma once
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // For each value, walks a stack of earlier values back until it finds one at
    // least as large. Everything smaller is popped, which is what the picture shows.
    EventLog next_greater(std::vector<int> data);
}
