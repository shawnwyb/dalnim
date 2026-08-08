#pragma once
#include <vector>
#include "core/event.hpp"
#include "core/timeline.hpp"

namespace dalnim {
    inline constexpr double kBoxSpacing = 60.0;
    inline constexpr double kSecondsPerEvent = 0.3;

    struct ArrayAnimation {
        std::vector<Timeline> x;
        double duration = 0.0;
    };

    ArrayAnimation build_array_animation(const std::vector<int>& input,
                                         const EventLog& log);
}
