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

    using Event = std::variant<Compare, Swap>;

    using EventLog = std::vector<Event>;
}
