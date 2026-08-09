#include "core/tree_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
namespace {
    // Every slot at depth d shares the width equally, so a node sits over the middle
    // of the span its subtree would occupy and no two edges ever cross.
    std::vector<Point> place(const Tree& tree, double& span_x, double& span_y) {
        std::vector<Point> positions(tree.present.size());
        if (tree.present.empty()) {
            span_x = 0.0;
            span_y = 0.0;
            return positions;
        }

        const std::size_t deepest = tree_depth(tree);
        std::size_t leaves = 1;
        for (std::size_t d = 0; d < deepest; ++d) {
            leaves *= 2;
        }

        span_x = static_cast<double>(leaves) * kLeafSpacing;
        span_y = static_cast<double>(deepest) * kTreeRowHeight;

        for (std::size_t slot = 0; slot < positions.size(); ++slot) {
            const std::size_t depth = depth_of(slot);
            std::size_t first_at_depth = 1;
            for (std::size_t d = 0; d < depth; ++d) {
                first_at_depth *= 2;
            }
            const std::size_t offset = slot - (first_at_depth - 1);
            const double share = span_x / static_cast<double>(first_at_depth);

            positions[slot].x = (static_cast<double>(offset) + 0.5) * share;
            positions[slot].y = static_cast<double>(depth) * kTreeRowHeight;
        }
        return positions;
    }
}

TreeAnimation build_tree_animation(Tree tree, EventLog log) {
    TreeAnimation anim;
    anim.duration = static_cast<double>(log.size());
    anim.positions = place(tree, anim.span_x, anim.span_y);
    anim.tree = std::move(tree);
    anim.log = std::move(log);
    return anim;
}

std::vector<std::optional<MarkKind>> tree_marks_at(const TreeAnimation& anim, double t) {
    return marks_at(anim.log, t, anim.tree.present.size());
}

std::optional<std::size_t> tree_highlight_at(const TreeAnimation& anim, double t) {
    return sticky_highlight_at(anim.log, t);
}

std::vector<int> tree_pile_at(const TreeAnimation& anim, double t) {
    return pile_at(anim.log, t);
}
}
