#include "core/set_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
    SetAnimation build_set_animation(std::vector<int> input, EventLog log) {
        SetAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.input = std::move(input);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> set_members_at(const SetAnimation& anim, double t) {
        return pile_at(anim.log, t);
    }

    std::optional<std::size_t> set_cursor_at(const SetAnimation& anim, double t) {
        return sticky_highlight_at(anim.log, t);
    }

    std::optional<std::pair<std::size_t, std::size_t>> set_compare_at(const SetAnimation& anim,
                                                                      double t) {
        // Sticky, so the match stays drawn while the answer is being marked and for
        // the rest of the run. There is only ever one, and it ends the walk.
        return sticky_compared_at(anim.log, t);
    }

    std::vector<std::optional<MarkKind>> set_marks_at(const SetAnimation& anim, double t) {
        return marks_at(anim.log, t, anim.input.size());
    }
}
