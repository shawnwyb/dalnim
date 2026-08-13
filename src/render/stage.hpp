#pragma once
#include <imgui.h>
#include <string>
#include <vector>
#include "render/camera.hpp"

namespace dalnim {
    // What every view shares: the area it may draw in, and the ink it draws with.
    // Anything only one view needs belongs in that view's own file instead.

    inline constexpr float kSidebarWidth = 330.0f;
    inline constexpr float kStageMargin = 40.0f;
    inline constexpr float kStageTop = 60.0f;
    inline constexpr float kStageBottom = 60.0f;
    inline constexpr float kMinScale = 0.12f;
    inline constexpr float kSmallestReadableFont = 9.0f;

    // Numbers on the stage are drawn larger than the sidebar text, which is sized
    // for reading up close. A label that outgrows its shape is dropped, not shrunk,
    // so raising this trades a little legibility against a few missing numbers.
    inline constexpr float kLabelScale = 1.5f;

    inline constexpr ImU32 kInk = IM_COL32(236, 236, 236, 255);
    inline constexpr ImU32 kEdge = IM_COL32(255, 255, 255, 255);
    inline constexpr ImU32 kLit = IM_COL32(250, 204, 21, 255);
    inline constexpr ImU32 kLitEdge = IM_COL32(255, 232, 130, 255);

    // What the run is pointing at when it stops. Warm rather than green, because
    // what an algorithm singles out is not always good news.
    inline constexpr ImU32 kAnswer = IM_COL32(239, 118, 108, 255);
    inline constexpr ImU32 kAnswerEdge = IM_COL32(255, 176, 168, 255);

    // Cameras onto the drawing area, which is whatever the sidebar does not cover.
    // Views never see the sidebar, so they cannot forget to allow for it.
    Camera fit_row_on_stage(double span_units);
    Camera fit_box_on_stage(double span_x_units, double span_y_units);

    // A stack of numbers resting on a line, brightest at the top. Both the stack
    // and the tree show one, driven by the same Push and Pop events.
    void draw_pile(ImDrawList* draw, const std::vector<int>& pile,
                   float left, float baseline, float side, float gap, float font_size);

    // Draws text centred in a box, or nothing at all if it would not fit.
    void draw_centred_label(ImDrawList* draw, const std::string& text, float font_size,
                            ImVec2 box_top_left, float box_width, float box_height, ImU32 colour);
}
