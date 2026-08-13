#include "render/set_view.hpp"

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

    // How far under the row the set sits, and how much of that is the word "set".
    constexpr float kSetDrop = 96.0f;
    constexpr float kCaptionLift = 26.0f;

    constexpr ImU32 kResting = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kMember = IM_COL32(150, 150, 150, 255);

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

    // A rounded pill, which is what keeps the set from reading as a second array.
    void draw_pill(ImDrawList* draw, ImVec2 top_left, float width, float height,
                   float font_size, const std::string& label, ImU32 fill, ImU32 edge) {
        const ImVec2 bottom_right{top_left.x + width, top_left.y + height};
        const float rounding = height * 0.5f;
        draw->AddRectFilled(top_left, bottom_right, fill, rounding);
        draw->AddRect(top_left, bottom_right, edge, rounding, 0, 1.5f);
        draw_centred_label(draw, label, font_size, top_left, width, height, IM_COL32_BLACK);
    }
}

void draw_set(const SetAnimation& anim, double t) {
    const std::size_t count = anim.input.size();
    if (count == 0) {
        return;
    }

    const ImVec2 screen = ImGui::GetIO().DisplaySize;
    const Camera camera = fit_row_on_stage(row_span(count));
    const float side = camera.length(kCellUnits);
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const std::optional<std::size_t> cursor = set_cursor_at(anim, t);
    const auto matched = set_compare_at(anim, t);
    const std::vector<std::optional<MarkKind>> marks = set_marks_at(anim, t);
    const std::vector<int> members = set_members_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    for (std::size_t i = 0; i < count; ++i) {
        const bool answered = marks[i] == MarkKind::Answer;
        const bool is_cursor = cursor.has_value() && *cursor == i;
        const bool in_match = matched.has_value() &&
                              (matched->first == i || matched->second == i);
        const bool passed = cursor.has_value() && i < *cursor;

        const ImVec2 top_left{camera.x(static_cast<double>(i) * kCellPitch), kRowTop};
        const ImU32 fill = answered ? kAnswer
                                    : ((is_cursor || in_match) ? kLit : (passed ? kInk : kResting));
        const bool dark = !(answered || is_cursor || in_match || passed);
        const ImU32 edge = answered ? kAnswerEdge : (dark ? kEdge : kLitEdge);
        draw_cell(draw, top_left, side, font_size, std::to_string(anim.input[i]),
                  fill, edge, dark);
    }

    // The value the lookup just found sitting in the set, so the hit is visible in
    // both places at once rather than only on the row.
    const bool hit = matched.has_value();
    const int hit_value = hit ? anim.input[matched->first] : 0;

    const float pill_w = side * 1.35f;
    const float pill_h = side * 0.72f;
    const float pill_gap = side * 0.24f;

    const float left_edge = kSidebarWidth + kStageMargin;
    const float usable = screen.x - left_edge - kStageMargin;
    const auto per_row = static_cast<std::size_t>((usable + pill_gap) / (pill_w + pill_gap));
    const std::size_t columns = per_row < 1 ? 1 : per_row;

    const float top = kRowTop + side + kSetDrop;

    draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                  ImVec2(left_edge, top - kCaptionLift), kMember, "set");

    for (std::size_t k = 0; k < members.size(); ++k) {
        const std::size_t column = k % columns;
        const std::size_t row = k / columns;
        const ImVec2 at{left_edge + static_cast<float>(column) * (pill_w + pill_gap),
                        top + static_cast<float>(row) * (pill_h + pill_gap)};

        // Every member is drawn alike unless the lookup just landed on it. A set has
        // no newest and no top, so nothing else here may vary with position.
        const bool struck = hit && members[k] == hit_value;
        draw_pill(draw, at, pill_w, pill_h, font_size, std::to_string(members[k]),
                  struck ? kAnswer : kMember, struck ? kAnswerEdge : kEdge);
    }
}
}
