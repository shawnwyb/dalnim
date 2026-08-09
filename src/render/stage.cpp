#include "render/stage.hpp"

#include <cfloat>

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
