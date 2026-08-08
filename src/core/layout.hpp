#pragma once
#include <vector>
#include "core/event.hpp"
#include "core/timeline.hpp"

namespace dalnim {
    inline constexpr double kBoxSpacing = 60.0;
    inline constexpr double kSecondsPerEvent = 0.3;

    struct ComparePair {
        double begin = 0.0;
        double end = 0.0;
        std::size_t box_a = 0;
        std::size_t box_b = 0;
    };

    struct ArrayAnimation {
        std::vector<Timeline> x;
        std::vector<ComparePair> compares;
        double duration = 0.0;
    };

    ArrayAnimation build_array_animation(const std::vector<int>& input,
                                         const EventLog& log);

    // Null when nothing is being compared at t. Points into anim, so it dies with it.
    const ComparePair* compare_at(const ArrayAnimation& anim, double t);
}
