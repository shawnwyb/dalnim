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
}
