#include "core/graph.hpp"
#include <algorithm>

namespace dalnim {
    std::size_t node_count(const Graph& graph) {
        return graph.neighbours.size();
    }

    bool has_edge(const Graph& graph, std::size_t from, std::size_t to) {
        if (from >= graph.neighbours.size()) {
            return false;
        }
        const std::vector<std::size_t>& out = graph.neighbours[from];
        return std::find(out.begin(), out.end(), to) != out.end();
    }

    std::size_t edge_count(const Graph& graph) {
        std::size_t ends = 0;
        for (const std::vector<std::size_t>& out : graph.neighbours) {
            ends += out.size();
        }
        // Each edge is recorded at both of its ends.
        return ends / 2;
    }
}
