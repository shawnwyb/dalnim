#pragma once
#include "core/graph_layout.hpp"

namespace dalnim {
    // Nodes on a ring, edges as chords, with the waiting queue along the bottom.
    void draw_graph(const GraphAnimation& anim, double t);
}
