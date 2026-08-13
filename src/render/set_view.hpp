#pragma once
#include "core/set_layout.hpp"

namespace dalnim {
    // The input as a row across the top, and the set below it as a loose spread of
    // pills. Pills rather than cells, and all the same brightness, because a set has
    // no order and no top the way the stack's pile does.
    void draw_set(const SetAnimation& anim, double t);
}
