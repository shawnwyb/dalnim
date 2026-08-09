#include "render/graph_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr ImU32 kResting = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kFrontier = IM_COL32(112, 112, 112, 255);
    constexpr ImU32 kVisited = IM_COL32(196, 196, 196, 255);
    constexpr ImU32 kEdgeLine = IM_COL32(96, 96, 96, 255);
    constexpr ImU32 kEdgeWalked = IM_COL32(210, 210, 210, 255);

    constexpr float kQueueSide = 30.0f;
    constexpr float kQueueGap = 4.0f;
    constexpr std::size_t kQueueShown = 14;

    ImU32 node_fill(bool lit, std::optional<MarkKind> mark) {
        if (lit) {
            return kLit;
        }
        if (mark == MarkKind::Visited) {
            return kVisited;
        }
        return mark == MarkKind::Frontier ? kFrontier : kResting;
    }

    void draw_queue(ImDrawList* draw, const std::vector<std::size_t>& waiting) {
        const float left = kSidebarWidth + 24.0f;
        const float top = ImGui::GetIO().DisplaySize.y - 54.0f;
        const float font_size = ImGui::GetFontSize() * 0.85f;

        draw->AddText(ImVec2(left, top - 20.0f), kEdge, "queue");

        const std::size_t shown = waiting.size() < kQueueShown ? waiting.size() : kQueueShown;
        for (std::size_t slot = 0; slot < shown; ++slot) {
            const float x = left + static_cast<float>(slot) * (kQueueSide + kQueueGap);
            const ImVec2 top_left{x, top};
            const ImVec2 bottom_right{x + kQueueSide, top + kQueueSide};
            const bool next_out = slot == 0;

            draw->AddRectFilled(top_left, bottom_right, next_out ? kInk : kFrontier, 4.0f);
            draw->AddRect(top_left, bottom_right, kEdge, 4.0f, 0, next_out ? 2.0f : 1.0f);
            draw_centred_label(draw, std::to_string(waiting[slot]), font_size,
                               top_left, kQueueSide, kQueueSide,
                               next_out ? IM_COL32_BLACK : kEdge);
        }

        if (waiting.size() > shown) {
            const float x = left + static_cast<float>(shown) * (kQueueSide + kQueueGap);
            draw->AddText(ImVec2(x, top + 8.0f), kEdge, "...");
        }
    }
}

void draw_graph(const GraphAnimation& anim, double t) {
    const std::size_t count = node_count(anim.graph);
    if (count == 0) {
        return;
    }

    const Camera camera = fit_box_on_stage(anim.span, anim.span);
    const float radius = camera.length(kGraphNodeUnits) * 0.5f;
    const float font_size = ImGui::GetFontSize() * camera.scale;

    const std::vector<std::optional<MarkKind>> marks = graph_marks_at(anim, t);
    const std::optional<std::size_t> lit = graph_highlight_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    // Edges first so the nodes cover their ends.
    for (std::size_t from = 0; from < count; ++from) {
        for (std::size_t to : anim.graph.neighbours[from]) {
            if (to < from) {
                continue;  // each edge is stored twice; draw it once
            }
            const bool both_seen = marks[from].has_value() && marks[to].has_value();
            draw->AddLine(ImVec2(camera.x(anim.positions[from].x), camera.y(anim.positions[from].y)),
                          ImVec2(camera.x(anim.positions[to].x), camera.y(anim.positions[to].y)),
                          both_seen ? kEdgeWalked : kEdgeLine, both_seen ? 2.0f : 1.0f);
        }
    }

    for (std::size_t node = 0; node < count; ++node) {
        const ImVec2 centre{camera.x(anim.positions[node].x), camera.y(anim.positions[node].y)};
        const bool is_lit = lit.has_value() && *lit == node;

        draw->AddCircleFilled(centre, radius, node_fill(is_lit, marks[node]));
        draw->AddCircle(centre, radius, is_lit ? kLitEdge : kEdge, 0, is_lit ? 2.5f : 1.5f);

        const bool dark = !is_lit && marks[node] != MarkKind::Visited;
        draw_centred_label(draw, std::to_string(node), font_size,
                           ImVec2(centre.x - radius, centre.y - radius),
                           radius * 2.0f, radius * 2.0f, dark ? kEdge : IM_COL32_BLACK);
    }

    draw_queue(draw, graph_frontier_at(anim, t));
}
}
