#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include "core/array_layout.hpp"
#include "core/event.hpp"
#include "core/intervals.hpp"
#include "core/timeline.hpp"

namespace dalnim {
    inline constexpr double kIntervalRowPitch = 54.0;
    inline constexpr double kIntervalBarHeight = 38.0;

    // How wide one step on the clock is drawn. Starts and ends are values, not
    // slots, so the picture is a real number line rather than a row of boxes.
    inline constexpr double kIntervalUnitsPerStep = 18.0;

    struct IntervalAnimation {
        // The intervals as they were typed. Sorting moves bars between rows and
        // never rewrites this, so a bar keeps its own start and end throughout.
        Intervals intervals;

        // Row of each bar over time, so a Swap slides two bars past each other
        // instead of teleporting them.
        std::vector<Timeline> row;

        std::vector<ComparePair> compares;
        EventLog log;
        double duration = 0.0;
        double span_x = 0.0;
        double span_y = 0.0;
        int earliest = 0;
    };

    IntervalAnimation build_interval_animation(Intervals intervals, EventLog log);

    // Where a value sits along the clock, measured from the earliest start.
    double interval_x_units(const IntervalAnimation& anim, int value);

    // Marks arrive naming slots, but a slot holds a different bar once the sort has
    // moved things, so these come back indexed by bar.
    std::vector<std::optional<MarkKind>> interval_marks_at(const IntervalAnimation& anim,
                                                           double t);

    // Null when nothing is being compared at t. Points into anim, so it dies with it.
    const ComparePair* interval_compare_at(const IntervalAnimation& anim, double t);
}
