#pragma once
#include <vector>

namespace dalnim {
    struct Keyframe {
        double time;
        double value;
    };

    class Timeline {
        public:
            void add(double time, double value);
            double sample(double t) const;
        private:
            std::vector<Keyframe> keyframes_;
    };
}