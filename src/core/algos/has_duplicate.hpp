#pragma once
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Walks the values once, keeping every one it has already seen in a set. The
    // first value already in the set is the answer, which is what the picture shows.
    EventLog has_duplicate(std::vector<int> data);
}
