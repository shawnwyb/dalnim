#pragma once
#include "core/stack_layout.hpp"

namespace dalnim {
    // The input as a row across the top, the pile growing upward beneath it.
    void draw_stack(const StackAnimation& anim, double t);
}
