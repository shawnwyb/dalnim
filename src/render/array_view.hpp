#pragma once
#include <vector>
#include "core/array_layout.hpp"

namespace dalnim {
    // Draws the array as bars, tallest for the largest value, at the moment `t`.
    void draw_array(const std::vector<int>& values, const ArrayAnimation& anim, double t);
}
