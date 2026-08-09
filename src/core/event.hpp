#pragma once
#include <cstddef>
#include <variant>
#include <vector>

namespace dalnim {
    struct Compare {
        std::size_t a;
        std::size_t b;
    };

    struct Swap {
        std::size_t a;
        std::size_t b;
    };

    struct Highlight {
        std::size_t index;
    };

    struct Set {
        std::size_t index;
        int value;
    };

    using Event = std::variant<Compare, Swap, Highlight, Set>;

    using EventLog = std::vector<Event>;

    // Event k owns the slot [k * kSecondsPerEvent, (k + 1) * kSecondsPerEvent).
    // Every view reads a log through this one convention.
    inline constexpr double kSecondsPerEvent = 0.3;
}
