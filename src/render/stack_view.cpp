#include "render/stack_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr double kCellUnits = 48.0;
    constexpr double kCellPitch = 56.0;
    constexpr float kRowTop = 90.0f;
    constexpr float kPileBottom = 70.0f;

    constexpr ImU32 kResting = IM_COL32(20, 20, 20, 255);

    double row_span(std::size_t count) {
        const double gap = kCellPitch - kCellUnits;
        return count == 0 ? 0.0 : static_cast<double>(count) * kCellPitch - gap;
    }

    void draw_cell(ImDrawList* draw, ImVec2 top_left, float side, float font_size,
                   const std::string& label, ImU32 fill, ImU32 edge, bool dark_fill) {
        const ImVec2 bottom_right{top_left.x + side, top_left.y + side};
        draw->AddRectFilled(top_left, bottom_right, fill, 4.0f);
        draw->AddRect(top_left, bottom_right, edge, 4.0f, 0, 1.5f);
        draw_centred_label(draw, label, font_size, top_left, side, side,
                           dark_fill ? kEdge : IM_COL32_BLACK);
    }
}

void draw_stack(const StackAnimation& anim, double t) {
    const std::size_t count = anim.input.size();
    if (count == 0) {
        return;
    }

    const ImVec2 screen = ImGui::GetIO().DisplaySize;
    const Camera camera = fit_row_on_stage(row_span(count));
    const float side = camera.length(kCellUnits);
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const std::optional<std::size_t> cursor = cursor_at(anim, t);
    const auto comparing = comparing_at(anim, t);
    const std::vector<int> pile = stack_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    for (std::size_t i = 0; i < count; ++i) {
        const bool is_cursor = cursor.has_value() && *cursor == i;
        const bool in_compare = comparing.has_value() &&
                                (comparing->first == i || comparing->second == i);
        const bool passed = cursor.has_value() && i < *cursor;

        const ImVec2 top_left{camera.x(static_cast<double>(i) * kCellPitch), kRowTop};
        const ImU32 fill = (is_cursor || in_compare) ? kLit : (passed ? kInk : kResting);
        const bool dark = !(is_cursor || in_compare || passed);
        draw_cell(draw, top_left, side, font_size, std::to_string(anim.input[i]),
                  fill, dark ? kEdge : kLitEdge, dark);
    }

    // The pile sits under the middle of the row and grows towards it.
    const float pile_x = camera.x(row_span(count) * 0.5) - side * 0.5f;
    draw_pile(draw, pile, pile_x, screen.y - kPileBottom, side,
              camera.length(kCellPitch - kCellUnits), font_size);
}
}
