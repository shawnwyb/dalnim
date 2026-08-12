#pragma once
#include <vector>

namespace dalnim {
    // A span with a start and an end, half open: a meeting ending at 10 and one
    // starting at 10 do not overlap, which is what the comparisons below rely on.
    struct Interval {
        int start;
        int end;
    };

    // Intervals in the order they were given. Algorithms are free to reorder them,
    // and record a Swap when they do, exactly as the array algorithms do.
    struct Intervals {
        std::vector<Interval> items;
    };
}
