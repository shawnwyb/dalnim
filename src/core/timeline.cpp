#include "core/timeline.hpp"
#include "core/ease.hpp"
#include <algorithm>
#include <cstddef>

namespace dalnim {
    void Timeline::add(double time, double value) {
        Keyframe k{.time=time, .value=value};
        keyframes_.push_back(k);
        std::sort(keyframes_.begin(), keyframes_.end(), [](const auto& a, const auto& b) {
            return a.time < b.time;
        });
    }

    double Timeline::sample(double t) const {
        if (keyframes_.empty()) {
            return 0.0;
        }
        if (t <= keyframes_.front().time) {
            return keyframes_.front().value;
        }
        if (t >= keyframes_.back().time) {
            return keyframes_.back().value;
        }
        // find the segment that contains t and lerp
        for (std::size_t i = 1; i < keyframes_.size(); ++i) {
            if (t > keyframes_[i].time) {
                continue;
            }
            const Keyframe& k1 = keyframes_[i - 1];
            const Keyframe& k2 = keyframes_[i];
            double alpha = smooth((t - k1.time) / (k2.time - k1.time));
            return k1.value + alpha * (k2.value - k1.value);
        }
        return keyframes_.back().value;
    }
}