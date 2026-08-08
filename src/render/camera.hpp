#pragma once

namespace dalnim {
    // Maps the abstract units core lays things out in onto screen pixels.
    struct Camera {
        float scale = 1.0f;
        float origin_x = 0.0f;

        float x(double units) const;
        float length(double units) const;
    };

    // Fits content `span_units` wide into a window, centred. Never enlarges past 1:1,
    // never shrinks past `min_scale`, and leaves `margin` pixels on each side.
    Camera fit_row(double span_units, float window_width, float margin, float min_scale);
}
