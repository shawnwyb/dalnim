#include "core/event.hpp"
#include <type_traits>

namespace dalnim {
    const char* mark_kind_name(MarkKind kind) {
        switch (kind) {
            case MarkKind::Visited: return "visited";
            case MarkKind::Frontier: return "frontier";
            case MarkKind::Answer: return "answer";
        }
        return "unknown";
    }

    std::string describe(const Event& event) {
        return std::visit([](const auto& e) -> std::string {
            using Kind = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<Kind, Compare>) {
                return "comparing " + std::to_string(e.a) + " and " + std::to_string(e.b);
            } else if constexpr (std::is_same_v<Kind, Swap>) {
                return "swapping " + std::to_string(e.a) + " and " + std::to_string(e.b);
            } else if constexpr (std::is_same_v<Kind, Highlight>) {
                return "visiting " + std::to_string(e.index);
            } else if constexpr (std::is_same_v<Kind, Mark>) {
                return "marking " + std::to_string(e.index) + " " + mark_kind_name(e.kind);
            } else if constexpr (std::is_same_v<Kind, Unmark>) {
                return "clearing " + std::string(mark_kind_name(e.kind)) + " from " + std::to_string(e.index);
            } else {
                return "setting " + std::to_string(e.index) + " to " + std::to_string(e.value);
            }
        }, event);
    }
}
