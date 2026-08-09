#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include "core/event.hpp"
#include "core/graph.hpp"
#include "core/point.hpp"

namespace dalnim {
    inline constexpr double kGraphNodeUnits = 56.0;
    inline constexpr double kGraphNodeSpacing = 100.0;
    inline constexpr double kGraphSmallestRadius = 130.0;

    struct GraphAnimation {
        Graph graph;
        std::vector<Point> positions;
        EventLog log;
        double duration = 0.0;
        double span = 0.0;
    };

    GraphAnimation build_graph_animation(Graph graph, EventLog log);

    std::vector<std::optional<MarkKind>> graph_marks_at(const GraphAnimation& anim, double t);
    std::optional<std::size_t> graph_highlight_at(const GraphAnimation& anim, double t);
    std::vector<std::size_t> graph_frontier_at(const GraphAnimation& anim, double t);
}
