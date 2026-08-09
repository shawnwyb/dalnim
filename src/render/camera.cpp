#include "render/camera.hpp"

namespace dalnim {
namespace {
    float clamp_scale(float wanted, float min_scale) {
        if (wanted > 1.0f) {
            return 1.0f;
        }
        return wanted < min_scale ? min_scale : wanted;
    }
}

float Camera::x(double units) const {
    return origin_x + static_cast<float>(units) * scale;
}

float Camera::y(double units) const {
    return origin_y + static_cast<float>(units) * scale;
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
    camera.scale = clamp_scale((window_width - 2.0f * margin) / span, min_scale);
    camera.origin_x = (window_width - span * camera.scale) * 0.5f;
    return camera;
}

Camera fit_box(double span_x_units, double span_y_units,
               float window_width, float window_height,
               float margin_x, float margin_y, float min_scale) {
    Camera camera;
    if (span_x_units <= 0.0 || span_y_units <= 0.0) {
        camera.origin_x = window_width * 0.5f;
        camera.origin_y = window_height * 0.5f;
        return camera;
    }

    const float span_x = static_cast<float>(span_x_units);
    const float span_y = static_cast<float>(span_y_units);
    const float by_width = (window_width - 2.0f * margin_x) / span_x;
    const float by_height = (window_height - 2.0f * margin_y) / span_y;

    // One scale for both axes, so squares stay square.
    camera.scale = clamp_scale(by_width < by_height ? by_width : by_height, min_scale);
    camera.origin_x = (window_width - span_x * camera.scale) * 0.5f;
    camera.origin_y = margin_y + (window_height - 2.0f * margin_y - span_y * camera.scale) * 0.5f;
    return camera;
}
}
