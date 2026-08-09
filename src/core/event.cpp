#include "core/event.hpp"

namespace dalnim {
    std::string describe(const Event& event) {
        return std::visit([](const auto& e) -> std::string {
            using Kind = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<Kind, Compare>) {
                return "comparing " + std::to_string(e.a) + " and " + std::to_string(e.b);
            } else if constexpr (std::is_same_v<Kind, Swap>) {
                return "swapping " + std::to_string(e.a) + " and " + std::to_string(e.b);
            } else if constexpr (std::is_same_v<Kind, Highlight>) {
                return "visiting " + std::to_string(e.index);
            } else {
                return "setting " + std::to_string(e.index) + " to " + std::to_string(e.value);
            }
        }, event);
    }
}
