#pragma once
#include "core/list_layout.hpp"

namespace dalnim {
    // Nodes in a row joined by arrows, with a loop drawn beneath if there is one.
    void draw_list(const ListAnimation& anim, double t);
}
