#include "core/algos/meeting_rooms.hpp"
#include <cstddef>
#include <utility>

namespace dalnim {
    EventLog can_attend_all(Intervals data) {
        EventLog log;
        std::vector<Interval>& items = data.items;

        // std::sort does its work out of sight, so the order is built by hand
        // instead: every probe and every exchange has to be an event to be watched.
        for (std::size_t i = 0; i + 1 < items.size(); ++i) {
            std::size_t earliest = i;
            for (std::size_t j = i + 1; j < items.size(); ++j) {
                log.push_back(Compare{.a = earliest, .b = j});
                if (items[j].start < items[earliest].start) {
                    earliest = j;
                }
            }
            if (earliest != i) {
                std::swap(items[i], items[earliest]);
                log.push_back(Swap{.a = i, .b = earliest});
            }
        }

        // Sorted by start, only neighbours can clash. Anything starting later than
        // this one begins even later, so one pass over the pairs settles it.
        for (std::size_t i = 1; i < items.size(); ++i) {
            log.push_back(Compare{.a = i - 1, .b = i});
            if (items[i].start < items[i - 1].end) {
                log.push_back(Mark{.index = i - 1, .kind = MarkKind::Answer});
                log.push_back(Mark{.index = i, .kind = MarkKind::Answer});
                return log;
            }
            log.push_back(Mark{.index = i - 1, .kind = MarkKind::Visited});
        }

        // Every pair held, so the last meeting is cleared too and nothing is marked
        // as the answer. An empty picture is how "yes, one room is enough" reads.
        if (!items.empty()) {
            log.push_back(Mark{.index = items.size() - 1, .kind = MarkKind::Visited});
        }
        return log;
    }
}
