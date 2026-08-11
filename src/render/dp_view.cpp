#include "render/dp_view.hpp"

#include <imgui.h>

#include <cstddef>
#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr double kCellUnits = 46.0;
    constexpr double kCellPitch = 50.0;

    constexpr ImU32 kBlank = IM_COL32(18, 18, 18, 255);
    constexpr ImU32 kDone = IM_COL32(196, 196, 196, 255);
    constexpr ImU32 kSource = IM_COL32(120, 220, 150, 255);
    constexpr ImU32 kGridLine = IM_COL32(70, 70, 70, 255);

    ImVec2 centre_of(const Camera& camera, std::size_t row, std::size_t col, float side) {
        return ImVec2(camera.x(static_cast<double>(col + 1) * kCellPitch) + side * 0.5f,
                      camera.y(static_cast<double>(row + 1) * kCellPitch) + side * 0.5f);
    }
}

void draw_dp(const DpAnimation& anim, double t) {
    const std::size_t width = dp_width(anim.table);
    const std::size_t height = dp_height(anim.table);
    if (width * height <= 1) {
        return;
    }

    // One extra row and column of room for the two words themselves.
    const double span_x = static_cast<double>(width + 1) * kCellPitch;
    const double span_y = static_cast<double>(height + 1) * kCellPitch;
    const Camera camera = fit_box_on_stage(span_x, span_y);
    const float side = camera.length(kCellUnits);
    const float font_size = ImGui::GetFontSize() * kLabelScale * camera.scale;

    const std::vector<int> values = dp_values_at(anim, t);
    const std::vector<bool> filled = dp_filled_at(anim, t);
    const std::optional<std::size_t> current = dp_current_at(anim, t);
    const auto source = dp_source_at(anim, t);

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    // The words, sitting outside the table along its top and left edges.
    for (std::size_t col = 0; col < anim.table.across.size(); ++col) {
        const std::string letter(1, anim.table.across[col]);
        const ImVec2 at{camera.x(static_cast<double>(col + 2) * kCellPitch),
                        camera.y(0.0)};
        draw_centred_label(draw, letter, font_size, at, side, side, kLit);
    }
    for (std::size_t row = 0; row < anim.table.down.size(); ++row) {
        const std::string letter(1, anim.table.down[row]);
        const ImVec2 at{camera.x(0.0),
                        camera.y(static_cast<double>(row + 2) * kCellPitch)};
        draw_centred_label(draw, letter, font_size, at, side, side, kLit);
    }

    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            const std::size_t cell = row * width + col;
            const ImVec2 top_left{camera.x(static_cast<double>(col + 1) * kCellPitch),
                                  camera.y(static_cast<double>(row + 1) * kCellPitch)};
            const ImVec2 bottom_right{top_left.x + side, top_left.y + side};

            const bool is_current = current.has_value() && *current == cell;
            const bool is_source = source.has_value() && source->first == cell &&
                                   source->second != cell;

            const ImU32 fill = is_current ? kLit
                                          : (is_source ? kSource : (filled[cell] ? kDone : kBlank));
            draw->AddRectFilled(top_left, bottom_right, fill, 3.0f * camera.scale);
            draw->AddRect(top_left, bottom_right, is_current ? kLitEdge : kGridLine,
                          3.0f * camera.scale, 0, is_current ? 2.0f : 1.0f);

            if (filled[cell]) {
                draw_centred_label(draw, std::to_string(values[cell]), font_size,
                                   top_left, side, side, IM_COL32_BLACK);
            }
        }
    }

    if (source.has_value() && source->first != source->second) {
        const ImVec2 from = centre_of(camera, source->first / width, source->first % width, side);
        const ImVec2 to = centre_of(camera, source->second / width, source->second % width, side);
        draw->AddLine(from, to, kSource, 2.0f);
        draw->AddCircleFilled(to, 4.0f, kSource);
    }
}
}
