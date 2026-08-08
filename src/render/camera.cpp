#include "render/camera.hpp"

namespace dalnim {
    float Camera::x(double units) const {
        return origin_x + static_cast<float>(units) * scale;
    }

    float Camera::length(double units) const {
        return static_cast<float>(units) * scale;
    }

    Camera fit_row(double span_units, float window_width, float margin, float min_scale) {
        Camera camera;
        if (span_units <= 0.0) {
            camera.origin_x = window_width * 0.5f;
            return camera;
        }

        const float span = static_cast<float>(span_units);
        const float usable = window_width - 2.0f * margin;
        const float wanted = usable / span;

        camera.scale = wanted > 1.0f ? 1.0f : (wanted < min_scale ? min_scale : wanted);
        camera.origin_x = (window_width - span * camera.scale) * 0.5f;
        return camera;
    }
}
