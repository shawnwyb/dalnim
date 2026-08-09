#include "render/stage.hpp"

#include <cfloat>
#include <cstddef>
#include <string>

namespace dalnim {
    Camera fit_row_on_stage(double span_units) {
        const float width = ImGui::GetIO().DisplaySize.x - kSidebarWidth;
        Camera camera = fit_row(span_units, width, kStageMargin, kMinScale);
        camera.origin_x += kSidebarWidth;
        return camera;
    }

    Camera fit_box_on_stage(double span_x_units, double span_y_units) {
        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        Camera camera = fit_box(span_x_units, span_y_units, screen.x - kSidebarWidth, screen.y,
                                kStageMargin, kStageTop, kMinScale);
        camera.origin_x += kSidebarWidth;
        return camera;
    }

    void draw_pile(ImDrawList* draw, const std::vector<int>& pile,
                   float left, float baseline, float side, float gap, float font_size) {
        constexpr ImU32 kBuried = IM_COL32(150, 150, 150, 255);

        for (std::size_t depth = 0; depth < pile.size(); ++depth) {
            const float lift = static_cast<float>(depth + 1) * (side + gap);
            const ImVec2 top_left{left, baseline - lift};
            const ImVec2 bottom_right{top_left.x + side, top_left.y + side};
            const bool on_top = depth + 1 == pile.size();

            draw->AddRectFilled(top_left, bottom_right, on_top ? kInk : kBuried, 4.0f);
            draw->AddRect(top_left, bottom_right, kEdge, 4.0f, 0, 1.5f);
            draw_centred_label(draw, std::to_string(pile[depth]), font_size,
                               top_left, side, side, IM_COL32_BLACK);
        }

        draw->AddLine(ImVec2(left - 6.0f, baseline), ImVec2(left + side + 6.0f, baseline),
                      kEdge, 2.0f);
    }

    void draw_centred_label(ImDrawList* draw, const std::string& text, float font_size,
                            ImVec2 box_top_left, float box_width, float box_height, ImU32 colour) {
        if (font_size < kSmallestReadableFont) {
            return;
        }
        ImFont* font = ImGui::GetFont();
        const ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());
        if (size.x > box_width || size.y > box_height) {
            return;
        }
        const ImVec2 at{box_top_left.x + (box_width - size.x) * 0.5f,
                        box_top_left.y + (box_height - size.y) * 0.5f};
        draw->AddText(font, font_size, at, colour, text.c_str());
    }
}
