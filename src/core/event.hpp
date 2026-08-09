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

    // A mark sticks to a cell until something removes it, unlike a highlight,
    // which lasts only for the event that raised it.
    enum class MarkKind { Visited, Frontier, Answer };

    struct Mark {
        std::size_t index;
        MarkKind kind;
    };

    struct Unmark {
        std::size_t index;
        MarkKind kind;
    };

    using Event = std::variant<Compare, Swap, Highlight, Set, Mark, Unmark>;

    using EventLog = std::vector<Event>;

    // Time is counted in events, not seconds: event k owns the span [k, k + 1).
    // How fast an event should pass on screen is the renderer's business.
    const char* mark_kind_name(MarkKind kind);

    std::string describe(const Event& event);
}
