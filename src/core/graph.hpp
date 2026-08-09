#pragma once
#include <cstddef>
#include <vector>

namespace dalnim {
    // Edges are undirected and stored on both ends, so a walk can go either way
    // along one however it was written down.
    struct Graph {
        std::vector<std::vector<std::size_t>> neighbours;
    };

    std::size_t node_count(const Graph& graph);
    bool has_edge(const Graph& graph, std::size_t from, std::size_t to);
    std::size_t edge_count(const Graph& graph);
}
