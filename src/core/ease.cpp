#include "core/ease.hpp"

namespace dalnim {
    double smooth(double alpha) {
        if (alpha <= 0.0) {
            return 0.0;
        }
        if (alpha >= 1.0) {
            return 1.0;
        }
        return alpha * alpha * (3.0 - 2.0 * alpha);
    }
}
