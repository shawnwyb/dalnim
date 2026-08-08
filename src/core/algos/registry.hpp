#pragma once
#include <span>
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    struct Algorithm {
        const char* name;
        EventLog (*run)(std::vector<int>);
    };

    // Everything the app can offer, in menu order. Adding one is a line in registry.cpp.
    std::span<const Algorithm> algorithms();
}
