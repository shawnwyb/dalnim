#include "core/algos/cycle_detect.hpp"

namespace dalnim {
    EventLog detect_cycle(LinkedList list) {
        EventLog log;
        if (list.values.empty()) {
            return log;
        }

        std::size_t slow = 0;
        std::size_t fast = 0;
        log.push_back(Compare{.a = slow, .b = fast});
        log.push_back(Mark{.index = slow, .kind = MarkKind::Visited});

        // A meeting happens within one lap of the loop, so this can only run out
        // when the list ends. The cap is a guard, not part of the method.
        const std::size_t most_steps = 4 * list.values.size() + 4;
        for (std::size_t step = 0; step < most_steps; ++step) {
            const std::optional<std::size_t> slow_next = next_of(list, slow);
            const std::optional<std::size_t> fast_once = next_of(list, fast);
            if (!slow_next.has_value() || !fast_once.has_value()) {
                break;
            }
            const std::optional<std::size_t> fast_twice = next_of(list, *fast_once);
            if (!fast_twice.has_value()) {
                break;
            }

            slow = *slow_next;
            fast = *fast_twice;

            log.push_back(Compare{.a = slow, .b = fast});
            log.push_back(Mark{.index = slow, .kind = MarkKind::Visited});

            if (slow == fast) {
                log.push_back(Mark{.index = slow, .kind = MarkKind::Answer});
                break;
            }
        }

        return log;
    }
}
