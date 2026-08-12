#pragma once
#include "core/interval_layout.hpp"

namespace dalnim {
    // Bars on a clock, one row each, laid left to right by when they start and
    // sliding between rows as the sort reorders them.
    void draw_intervals(const IntervalAnimation& anim, double t);
}
