#pragma once
#include "core/event.hpp"
#include "core/intervals.hpp"

namespace dalnim {
    // Can one room hold every meeting? Sorts by start time, then looks for a
    // meeting that begins before the one before it ends. The two that clash are
    // left marked as the answer; a schedule that works marks nothing.
    EventLog can_attend_all(Intervals data);
}
