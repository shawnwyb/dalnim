#include "core/graph_layout.hpp"
#include <cmath>
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
namespace {
    // A graph has no root and may have cycles, so unlike a tree its positions cannot
    // come from the shape of the data. A ring gives every node the same room and can
    // never fold over itself, whatever the edges do.
    std::vector<Point> place(const Graph& graph, double& span) {
        const std::size_t count = node_count(graph);
        std::vector<Point> positions(count);
        if (count == 0) {
            span = 0.0;
            return positions;
        }

        const double wanted = static_cast<double>(count) * kGraphNodeSpacing / (2.0 * M_PI);
        const double radius = wanted < kGraphSmallestRadius ? kGraphSmallestRadius : wanted;
        span = 2.0 * radius + kGraphNodeUnits;

        for (std::size_t i = 0; i < count; ++i) {
            // Starting at the top reads better than starting at the right.
            const double angle = 2.0 * M_PI * static_cast<double>(i) /
                                     static_cast<double>(count) - M_PI / 2.0;
            positions[i].x = radius + radius * std::cos(angle) + kGraphNodeUnits / 2.0;
            positions[i].y = radius + radius * std::sin(angle) + kGraphNodeUnits / 2.0;
        }
        return positions;
    }
}

GraphAnimation build_graph_animation(Graph graph, EventLog log) {
    GraphAnimation anim;
    anim.duration = static_cast<double>(log.size());
    anim.positions = place(graph, anim.span);
    anim.graph = std::move(graph);
    anim.log = std::move(log);
    return anim;
}

std::vector<std::optional<MarkKind>> graph_marks_at(const GraphAnimation& anim, double t) {
    return marks_at(anim.log, t, node_count(anim.graph));
}

std::optional<std::size_t> graph_highlight_at(const GraphAnimation& anim, double t) {
    return sticky_highlight_at(anim.log, t);
}

std::vector<std::size_t> graph_frontier_at(const GraphAnimation& anim, double t) {
    return marked_in_order_at(anim.log, t, MarkKind::Frontier);
}
}
