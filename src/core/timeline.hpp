#pragma once
#include <vector>

namespace dalnim {
    struct Keyframe {
        double time;
        double value;
    };

    class Timeline {
        public:
            std::vector<Keyframe> keyframes;
    };
}