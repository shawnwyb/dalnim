#include "core/stack_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
    StackAnimation build_stack_animation(std::vector<int> input, EventLog log) {
        StackAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.input = std::move(input);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> stack_at(const StackAnimation& anim, double t) {
        return pile_at(anim.log, t);
    }

    std::optional<std::size_t> cursor_at(const StackAnimation& anim, double t) {
        return sticky_highlight_at(anim.log, t);
    }

    std::optional<std::pair<std::size_t, std::size_t>> comparing_at(const StackAnimation& anim,
                                                                   double t) {
        return compared_at(anim.log, t);
    }
}
