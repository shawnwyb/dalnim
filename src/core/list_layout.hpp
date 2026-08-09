#pragma once
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "core/event.hpp"
#include "core/linked_list.hpp"

namespace dalnim {
    inline constexpr double kListPitch = 96.0;
    inline constexpr double kListNodeUnits = 60.0;

    struct ListAnimation {
        LinkedList list;
        EventLog log;
        double duration = 0.0;
        double span_x = 0.0;
    };

    ListAnimation build_list_animation(LinkedList list, EventLog log);

    std::vector<std::optional<MarkKind>> list_marks_at(const ListAnimation& anim, double t);

    // Where the two pointers are, slow first.
    std::optional<std::pair<std::size_t, std::size_t>> pointers_at(const ListAnimation& anim,
                                                                   double t);
}
