#pragma once
#include "core/event.hpp"
#include "core/linked_list.hpp"

namespace dalnim {
    // Floyd's two pointers: one steps once, the other twice. If they ever land on
    // the same node there is a loop. Each Compare names where the two are.
    EventLog detect_cycle(LinkedList list);
}
