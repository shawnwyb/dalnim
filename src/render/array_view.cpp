#include "render/array_view.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstddef>
#include <string>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    // Core spaces boxes kBoxSpacing apart; a bar covers most of that gap.
    constexpr float kBoxUnits = 48.0f;
    constexpr float kShortestBar = 0.18f;

    double row_span(std::size_t count) {
        return count == 0 ? 0.0 : static_cast<double>(count - 1) * kBoxSpacing + kBoxUnits;
    }

    // Where a value sits between the smallest and largest on screen, 0..1.
    float height_fraction(int value, int lowest, int highest) {
        if (highest == lowest) {
            return 1.0f;
        }
        const float span = static_cast<float>(highest) - static_cast<float>(lowest);
        const float over = static_cast<float>(value) - static_cast<float>(lowest);
        return kShortestBar + (over / span) * (1.0f - kShortestBar);
    }
}

void draw_array(const std::vector<int>& values, const ArrayAnimation& anim, double t) {
    const std::size_t count = values.size();
    if (count == 0) {
        return;
    }

    const ImVec2 screen = ImGui::GetIO().DisplaySize;
    const Camera camera = fit_row_on_stage(row_span(count));

    const float bar_width = camera.length(kBoxUnits);
    const float baseline = screen.y - kStageBottom;
    const float tallest = baseline - kStageTop;
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const auto bounds = std::minmax_element(values.begin(), values.end());
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    const ComparePair* comparing = compare_at(anim, t);

    for (std::size_t i = 0; i < count; ++i) {
        const bool lit = comparing != nullptr &&
                         (comparing->box_a == i || comparing->box_b == i);

        const float x = camera.x(anim.x[i].sample(t));
        const float height = tallest * height_fraction(values[i], *bounds.first, *bounds.second);
        const ImVec2 top_left{x, baseline - height};
        const ImVec2 bottom_right{x + bar_width, baseline};

        draw->AddRectFilled(top_left, bottom_right, lit ? kLit : kInk, 4.0f * camera.scale);
        draw->AddRect(top_left, bottom_right, lit ? kLitEdge : kEdge,
                      4.0f * camera.scale, 0, lit ? 2.5f : 1.0f);

        draw_centred_label(draw, std::to_string(values[i]), font_size,
                           top_left, bar_width, height, IM_COL32_BLACK);
    }
}
}
