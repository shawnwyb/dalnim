#include "core/grid_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
    GridAnimation build_grid_animation(Grid initial, EventLog log) {
        GridAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.initial = std::move(initial);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> grid_values_at(const GridAnimation& anim, double t) {
        return values_at(anim.initial.cells, anim.log, t);
    }

    std::vector<std::optional<MarkKind>> marks_at(const GridAnimation& anim, double t) {
        return marks_at(anim.log, t, anim.initial.cells.size());
    }

    std::optional<std::size_t> highlighted_at(const GridAnimation& anim, double t) {
        return highlight_at(anim.log, t);
    }
}
