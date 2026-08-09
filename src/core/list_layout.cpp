#include "core/list_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
    ListAnimation build_list_animation(LinkedList list, EventLog log) {
        ListAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.span_x = list.values.empty()
                          ? 0.0
                          : static_cast<double>(list.values.size() - 1) * kListPitch +
                                kListNodeUnits;
        anim.list = std::move(list);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<std::optional<MarkKind>> list_marks_at(const ListAnimation& anim, double t) {
        return marks_at(anim.log, t, anim.list.values.size());
    }

    std::optional<std::pair<std::size_t, std::size_t>> pointers_at(const ListAnimation& anim,
                                                                   double t) {
        // The pointers hold their places between comparisons rather than blinking
        // out on the events in between.
        return sticky_compared_at(anim.log, t);
    }
}
