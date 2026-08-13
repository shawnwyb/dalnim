#pragma once
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Nothing here moves, so the log is kept and replayed rather than turned into
    // timelines. The same shape the stack uses, for the same reason.
    struct SetAnimation {
        std::vector<int> input;
        EventLog log;
        double duration = 0.0;
    };

    SetAnimation build_set_animation(std::vector<int> input, EventLog log);

    // What the set holds at t. Insertion order is how they come back, because that
    // is all a log can know; a set has no order of its own and the view draws none.
    std::vector<int> set_members_at(const SetAnimation& anim, double t);

    // Which value is being looked up. Holds until the next one, because it marks a
    // whole step rather than a single event.
    std::optional<std::size_t> set_cursor_at(const SetAnimation& anim, double t);

    // The pair the lookup matched, if it has matched yet, as indices into the input.
    std::optional<std::pair<std::size_t, std::size_t>> set_compare_at(const SetAnimation& anim,
                                                                      double t);

    std::vector<std::optional<MarkKind>> set_marks_at(const SetAnimation& anim, double t);
}
