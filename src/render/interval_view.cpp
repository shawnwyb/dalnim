#include "render/interval_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr ImU32 kSeen = IM_COL32(150, 150, 150, 255);
    constexpr ImU32 kAxis = IM_COL32(90, 90, 90, 255);

    // What the run is pointing at when it stops. Warm rather than green, because
    // the intervals an interval algorithm singles out are not always good news.
    constexpr ImU32 kAnswer = IM_COL32(239, 118, 108, 255);
    constexpr ImU32 kAnswerEdge = IM_COL32(255, 176, 168, 255);
}

void draw_intervals(const IntervalAnimation& anim, double t) {
    const std::size_t count = anim.intervals.items.size();
    if (count == 0) {
        return;
    }

    const Camera camera = fit_box_on_stage(anim.span_x, anim.span_y);
    const float bar_height = camera.length(kIntervalBarHeight);
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const std::vector<std::optional<MarkKind>> marks = interval_marks_at(anim, t);
    const ComparePair* comparing = interval_compare_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    // The clock the bars rest on, drawn first so nothing sits on top of a bar.
    const float axis_y = camera.y(anim.span_y + kIntervalRowPitch * 0.4);
    draw->AddLine(ImVec2(camera.x(0.0), axis_y), ImVec2(camera.x(anim.span_x), axis_y),
                  kAxis, 1.5f);

    for (std::size_t bar = 0; bar < count; ++bar) {
        const Interval& item = anim.intervals.items[bar];

        const float left = camera.x(interval_x_units(anim, item.start));
        const float right = camera.x(interval_x_units(anim, item.end));
        const float top = camera.y(anim.row[bar].sample(t));

        const bool lit = comparing != nullptr &&
                         (comparing->box_a == bar || comparing->box_b == bar);
        const bool answered = marks[bar] == MarkKind::Answer;
        const bool seen = marks[bar] == MarkKind::Visited;

        const ImU32 fill = answered ? kAnswer : (lit ? kLit : (seen ? kSeen : kInk));
        const ImU32 edge = answered ? kAnswerEdge : (lit ? kLitEdge : kEdge);

        const ImVec2 top_left{left, top};
        const ImVec2 bottom_right{right, top + bar_height};

        draw->AddRectFilled(top_left, bottom_right, fill, 4.0f * camera.scale);
        draw->AddRect(top_left, bottom_right, edge, 4.0f * camera.scale, 0,
                      lit || answered ? 2.5f : 1.0f);

        // A zero-width meeting still has to say when it was, so the label goes
        // beside a bar too narrow to hold it rather than being dropped.
        const std::string label = std::to_string(item.start) + "-" + std::to_string(item.end);
        const float width = right - left;
        if (width >= bar_height) {
            draw_centred_label(draw, label, font_size, top_left, width, bar_height,
                               IM_COL32_BLACK);
        } else if (font_size >= kSmallestReadableFont) {
            draw->AddText(ImGui::GetFont(), font_size, ImVec2(right + 6.0f, top), kEdge,
                          label.c_str());
        }
    }
}
}
