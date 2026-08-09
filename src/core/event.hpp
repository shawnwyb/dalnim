#pragma once
#include <cstddef>
#include <string>
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

    // Time is counted in events, not seconds: event k owns the span [k, k + 1).
    // How fast an event should pass on screen is the renderer's business.
    std::string describe(const Event& event);
}
