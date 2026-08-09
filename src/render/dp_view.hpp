#pragma once
#include "core/dp_layout.hpp"

namespace dalnim {
    // The table with both words down its edges, and an arrow from the cell the
    // current answer came from.
    void draw_dp(const DpAnimation& anim, double t);
}
