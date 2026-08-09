#pragma once
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "core/dp_table.hpp"
#include "core/event.hpp"

namespace dalnim {
    struct DpAnimation {
        DpTable table;
        EventLog log;
        double duration = 0.0;
    };

    DpAnimation build_dp_animation(DpTable table, EventLog log);

    std::vector<int> dp_values_at(const DpAnimation& anim, double t);

    // A cell holds nothing until it has been written to, so an unfilled cell is
    // blank rather than zero.
    std::vector<bool> dp_filled_at(const DpAnimation& anim, double t);

    std::optional<std::size_t> dp_current_at(const DpAnimation& anim, double t);

    // Which cell the current one took its answer from, and which cell that is.
    std::optional<std::pair<std::size_t, std::size_t>> dp_source_at(const DpAnimation& anim,
                                                                    double t);
}
