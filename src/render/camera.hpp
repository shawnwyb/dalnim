#pragma once

namespace dalnim {
    // Maps the abstract units core lays things out in onto screen pixels.
    struct Camera {
        float scale = 1.0f;
        float origin_x = 0.0f;
        float origin_y = 0.0f;

        float x(double units) const;
        float y(double units) const;
        float length(double units) const;
    };

    // Fits content `span_units` wide into a window, centred horizontally. Never enlarges
    // past 1:1, never shrinks past `min_scale`, and leaves `margin` pixels on each side.
    Camera fit_row(double span_units, float window_width, float margin, float min_scale);

    // The same, fitting both axes at one shared scale so the content is not distorted.
    Camera fit_box(double span_x_units, double span_y_units,
                   float window_width, float window_height,
                   float margin_x, float margin_y, float min_scale);
}
