#pragma once
#include <cstddef>
#include "core/event.hpp"
#include "core/graph.hpp"

namespace dalnim {
    // Breadth-first from `start`, so nodes are reached in order of how many edges
    // away they are. Queued nodes are marked frontier, reached ones visited.
    EventLog graph_bfs(Graph graph, std::size_t start);
}
