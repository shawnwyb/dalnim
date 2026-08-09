#include "render/list_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr ImU32 kResting = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kSeen = IM_COL32(150, 150, 150, 255);
    constexpr ImU32 kFound = IM_COL32(120, 220, 150, 255);
    constexpr ImU32 kArrow = IM_COL32(140, 140, 140, 255);

    void arrow(ImDrawList* draw, ImVec2 from, ImVec2 to, ImU32 colour) {
        draw->AddLine(from, to, colour, 1.5f);
        const ImVec2 head{to.x - 7.0f, to.y};
        draw->AddTriangleFilled(to, ImVec2(head.x, head.y - 4.0f),
                                ImVec2(head.x, head.y + 4.0f), colour);
    }

    void label_pointer(ImDrawList* draw, const char* text, ImVec2 at, ImU32 colour) {
        draw->AddText(at, colour, text);
    }
}

void draw_list(const ListAnimation& anim, double t) {
    const std::size_t count = anim.list.values.size();
    if (count == 0) {
        return;
    }

    const ImVec2 screen = ImGui::GetIO().DisplaySize;
    const Camera camera = fit_row_on_stage(anim.span_x);
    const float side = camera.length(kListNodeUnits);
    const float font_size = ImGui::GetFontSize() * camera.scale;
    const float row_y = screen.y * 0.45f;

    const std::vector<std::optional<MarkKind>> marks = list_marks_at(anim, t);
    const auto pointers = pointers_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    for (std::size_t i = 0; i < count; ++i) {
        const float x = camera.x(static_cast<double>(i) * kListPitch);
        const ImVec2 top_left{x, row_y};
        const ImVec2 bottom_right{x + side, row_y + side};

        const bool is_slow = pointers.has_value() && pointers->first == i;
        const bool is_fast = pointers.has_value() && pointers->second == i;
        const bool found = marks[i] == MarkKind::Answer;
        const bool seen = marks[i] == MarkKind::Visited;

        const ImU32 fill = found ? kFound
                                 : ((is_slow || is_fast) ? kLit : (seen ? kSeen : kResting));
        const bool dark = !(found || is_slow || is_fast || seen);

        draw->AddRectFilled(top_left, bottom_right, fill, 6.0f * camera.scale);
        draw->AddRect(top_left, bottom_right, (is_slow || is_fast) ? kLitEdge : kEdge,
                      6.0f * camera.scale, 0, (is_slow || is_fast) ? 2.5f : 1.5f);
        draw_centred_label(draw, std::to_string(anim.list.values[i]), font_size,
                           top_left, side, side, dark ? kEdge : IM_COL32_BLACK);

        if (is_slow) {
            label_pointer(draw, "slow", ImVec2(x, row_y - 22.0f), kLitEdge);
        }
        if (is_fast) {
            label_pointer(draw, "fast", ImVec2(x, row_y + side + 8.0f), kLitEdge);
        }

        if (i + 1 < count) {
            const float next_x = camera.x(static_cast<double>(i + 1) * kListPitch);
            arrow(draw, ImVec2(x + side + 4.0f, row_y + side * 0.5f),
                  ImVec2(next_x - 4.0f, row_y + side * 0.5f), kArrow);
        }
    }

    if (has_cycle(anim.list)) {
        const float from_x = camera.x(static_cast<double>(count - 1) * kListPitch) + side * 0.5f;
        const float to_x = camera.x(static_cast<double>(*anim.list.cycle_to) * kListPitch) +
                           side * 0.5f;
        const float dip = row_y + side + 60.0f;

        draw->AddBezierCubic(ImVec2(from_x, row_y + side), ImVec2(from_x, dip),
                             ImVec2(to_x, dip), ImVec2(to_x, row_y + side), kArrow, 1.5f, 0);
        draw->AddTriangleFilled(ImVec2(to_x, row_y + side),
                                ImVec2(to_x - 4.0f, row_y + side + 8.0f),
                                ImVec2(to_x + 4.0f, row_y + side + 8.0f), kArrow);
    }
}
}
