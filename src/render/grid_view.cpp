#include "render/grid_view.hpp"

#include <imgui.h>

#include <string>
#include <vector>

#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr double kCellUnits = 48.0;
    constexpr double kCellPitch = 54.0;

    constexpr ImU32 kEmpty = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kWall = IM_COL32(88, 88, 88, 255);
    constexpr ImU32 kFrontier = IM_COL32(112, 112, 112, 255);
    constexpr ImU32 kVisited = IM_COL32(196, 196, 196, 255);

    // Whatever the algorithm is touching right now wins, then any lasting mark,
    // then the cell's own value.
    ImU32 cell_fill(int value, bool lit, std::optional<MarkKind> mark) {
        if (lit) {
            return kLit;
        }
        if (mark == MarkKind::Visited) {
            return kVisited;
        }
        if (mark == MarkKind::Frontier) {
            return kFrontier;
        }
        if (mark == MarkKind::Answer) {
            return kLit;
        }
        if (value == 0) {
            return kEmpty;
        }
        return value == 1 ? kInk : kWall;
    }

    bool fill_is_dark(int value, bool lit, std::optional<MarkKind> mark) {
        if (lit || mark == MarkKind::Answer || mark == MarkKind::Visited) {
            return false;
        }
        if (mark == MarkKind::Frontier) {
            return true;
        }
        return value == 0;
    }
}

std::optional<std::size_t> draw_grid(const GridAnimation& anim, double t, std::size_t start) {
    const Grid& grid = anim.initial;
    if (grid.width == 0 || grid.height == 0) {
        return std::nullopt;
    }

    const double gap = kCellPitch - kCellUnits;
    const double span_x = static_cast<double>(grid.width) * kCellPitch - gap;
    const double span_y = static_cast<double>(grid.height) * kCellPitch - gap;
    const Camera camera = fit_box_on_stage(span_x, span_y);

    const std::vector<int> cells = grid_values_at(anim, t);
    const std::vector<std::optional<MarkKind>> marks = marks_at(anim, t);
    const std::optional<std::size_t> lit = highlighted_at(anim, t);
    const float side = camera.length(kCellUnits);
    const float font_size = ImGui::GetFontSize() * camera.scale;

    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    const ImVec2 mouse = ImGui::GetMousePos();
    const bool mouse_is_ours = !ImGui::GetIO().WantCaptureMouse;
    std::optional<std::size_t> clicked;

    for (std::size_t i = 0; i < cells.size(); ++i) {
        const std::size_t row = i / grid.width;
        const std::size_t col = i % grid.width;
        const ImVec2 top_left{
            camera.x(static_cast<double>(col) * kCellPitch),
            camera.y(static_cast<double>(row) * kCellPitch),
        };
        const ImVec2 bottom_right{top_left.x + side, top_left.y + side};

        const bool hovered = mouse_is_ours &&
                             mouse.x >= top_left.x && mouse.x < bottom_right.x &&
                             mouse.y >= top_left.y && mouse.y < bottom_right.y;
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            clicked = i;
        }

        const bool is_lit = lit.has_value() && *lit == i;

        draw->AddRectFilled(top_left, bottom_right, cell_fill(cells[i], is_lit, marks[i]),
                            4.0f * camera.scale);
        draw->AddRect(top_left, bottom_right, hovered ? kLitEdge : kEdge,
                      4.0f * camera.scale, 0, (is_lit || hovered) ? 2.5f : 1.0f);
        if (i == start) {
            draw->AddRect(ImVec2(top_left.x - 3.0f, top_left.y - 3.0f),
                          ImVec2(bottom_right.x + 3.0f, bottom_right.y + 3.0f),
                          kLitEdge, 5.0f * camera.scale, 0, 1.5f);
        }

        const ImU32 ink = fill_is_dark(cells[i], is_lit, marks[i]) ? kEdge : IM_COL32_BLACK;
        draw_centred_label(draw, std::to_string(cells[i]), font_size, top_left, side, side, ink);
    }

    return clicked;
}
}
