#pragma once
#include "core/tree_layout.hpp"

namespace dalnim {
    // Nodes joined by edges, with the pile of pending nodes down the left.
    void draw_tree(const TreeAnimation& anim, double t);
}
