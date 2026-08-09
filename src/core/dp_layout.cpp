#include "core/dp_layout.hpp"
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
    DpAnimation build_dp_animation(DpTable table, EventLog log) {
        DpAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.table = std::move(table);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> dp_values_at(const DpAnimation& anim, double t) {
        return values_at(std::vector<int>(dp_cells(anim.table), 0), anim.log, t);
    }

    std::vector<bool> dp_filled_at(const DpAnimation& anim, double t) {
        return written_at(anim.log, t, dp_cells(anim.table));
    }

    std::optional<std::size_t> dp_current_at(const DpAnimation& anim, double t) {
        return sticky_highlight_at(anim.log, t);
    }

    std::optional<std::pair<std::size_t, std::size_t>> dp_source_at(const DpAnimation& anim,
                                                                    double t) {
        return sticky_compared_at(anim.log, t);
    }
}
