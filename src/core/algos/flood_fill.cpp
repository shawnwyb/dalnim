#include "core/algos/flood_fill.hpp"

namespace dalnim {
    EventLog flood_fill(Grid grid, std::size_t start, int fill_value) {
        EventLog log;
        if (!grid_is_well_formed(grid) || grid.width == 0 || start >= grid.cells.size()) {
            return log;
        }

        const int target = grid.cells[start];
        if (target == fill_value) {
            return log;
        }

        std::vector<std::size_t> pending{start};
        while (!pending.empty()) {
            const std::size_t i = pending.back();
            pending.pop_back();
            if (grid.cells[i] != target) {
                continue;
            }

            log.push_back(Highlight{.index = i});
            grid.cells[i] = fill_value;
            log.push_back(Set{.index = i, .value = fill_value});

            for (std::size_t j : neighbours(grid, i)) {
                pending.push_back(j);
            }
        }

        return log;
    }
}
