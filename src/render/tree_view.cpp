#include "render/tree_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr double kNodeRadius = 24.0;
    constexpr ImU32 kResting = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kSeen = IM_COL32(196, 196, 196, 255);
    constexpr ImU32 kBranch = IM_COL32(110, 110, 110, 255);

    constexpr float kPileSide = 34.0f;
    constexpr float kPileGap = 4.0f;
}

void draw_tree(const TreeAnimation& anim, double t) {
    if (anim.positions.empty() || !has_node(anim.tree, 0)) {
        return;
    }

    const Camera camera = fit_box_on_stage(anim.span_x + 2.0 * kNodeRadius,
                                           anim.span_y + 2.0 * kNodeRadius);
    const float radius = camera.length(kNodeRadius);
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const std::vector<std::optional<MarkKind>> marks = tree_marks_at(anim, t);
    const std::optional<std::size_t> lit = tree_highlight_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    // Edges first, so the nodes sit on top of them.
    for (std::size_t slot = 0; slot < anim.positions.size(); ++slot) {
        if (!has_node(anim.tree, slot) || slot == 0) {
            continue;
        }
        const std::size_t up = parent_of(slot);
        const ImVec2 from{camera.x(anim.positions[up].x), camera.y(anim.positions[up].y)};
        const ImVec2 to{camera.x(anim.positions[slot].x), camera.y(anim.positions[slot].y)};
        draw->AddLine(from, to, kBranch, 1.5f);
    }

    for (std::size_t slot = 0; slot < anim.positions.size(); ++slot) {
        if (!has_node(anim.tree, slot)) {
            continue;
        }

        const ImVec2 centre{camera.x(anim.positions[slot].x), camera.y(anim.positions[slot].y)};
        const bool is_lit = lit.has_value() && *lit == slot;
        const bool seen = marks[slot] == MarkKind::Visited;

        const ImU32 fill = is_lit ? kLit : (seen ? kSeen : kResting);
        draw->AddCircleFilled(centre, radius, fill);
        draw->AddCircle(centre, radius, is_lit ? kLitEdge : kEdge, 0, is_lit ? 2.5f : 1.5f);

        const ImVec2 top_left{centre.x - radius, centre.y - radius};
        draw_centred_label(draw, std::to_string(anim.tree.values[slot]), font_size,
                           top_left, radius * 2.0f, radius * 2.0f,
                           (is_lit || seen) ? IM_COL32_BLACK : kEdge);
    }

    draw_pile(draw, tree_pile_at(anim, t), kSidebarWidth + 24.0f,
              ImGui::GetIO().DisplaySize.y - 60.0f, kPileSide, kPileGap,
              ImGui::GetFontSize() * 0.9f);
}
}
