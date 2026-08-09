#include "core/linked_list.hpp"

namespace dalnim {
    std::optional<std::size_t> next_of(const LinkedList& list, std::size_t node) {
        if (node + 1 < list.values.size()) {
            return node + 1;
        }
        if (node + 1 == list.values.size() && list.cycle_to.has_value() &&
            *list.cycle_to < list.values.size()) {
            return list.cycle_to;
        }
        return std::nullopt;
    }

    bool has_cycle(const LinkedList& list) {
        return !list.values.empty() && list.cycle_to.has_value() &&
               *list.cycle_to < list.values.size();
    }
}
