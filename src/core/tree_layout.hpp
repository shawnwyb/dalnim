#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include "core/event.hpp"
#include "core/tree.hpp"

namespace dalnim {
    inline constexpr double kLeafSpacing = 70.0;
    inline constexpr double kTreeRowHeight = 80.0;

    struct Point {
        double x = 0.0;
        double y = 0.0;
    };

    // The first view whose positions come from the shape of the data rather than
    // from an index. Nothing here moves, so the positions are worked out once.
    struct TreeAnimation {
        Tree tree;
        std::vector<Point> positions;
        EventLog log;
        double duration = 0.0;
        double span_x = 0.0;
        double span_y = 0.0;
    };

    TreeAnimation build_tree_animation(Tree tree, EventLog log);

    std::vector<std::optional<MarkKind>> tree_marks_at(const TreeAnimation& anim, double t);
    std::optional<std::size_t> tree_highlight_at(const TreeAnimation& anim, double t);
    std::vector<int> tree_pile_at(const TreeAnimation& anim, double t);
}
