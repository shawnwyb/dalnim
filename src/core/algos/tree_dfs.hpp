#pragma once
#include "core/event.hpp"
#include "core/tree.hpp"

namespace dalnim {
    // Depth-first, left child before right. Push and Pop track the pending nodes,
    // which is what the pile beside the tree shows.
    EventLog tree_dfs(Tree tree);
}
