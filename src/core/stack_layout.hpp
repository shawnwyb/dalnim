#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Like a grid, nothing here moves, so the log is kept and replayed rather than
    // turned into timelines.
    struct StackAnimation {
        std::vector<int> input;
        EventLog log;
        double duration = 0.0;
    };

    StackAnimation build_stack_animation(std::vector<int> input, EventLog log);

    // The pile as of time t, bottom first.
    std::vector<int> stack_at(const StackAnimation& anim, double t);

    // Which input value the algorithm is working on. Unlike a highlight on a grid,
    // this one persists until the next one, because it marks a whole step.
    std::optional<std::size_t> cursor_at(const StackAnimation& anim, double t);

    // The pair being compared right now, if any, as indices into the input.
    std::optional<std::pair<std::size_t, std::size_t>> comparing_at(const StackAnimation& anim,
                                                                   double t);
}
