#include "core/algos/bfs.hpp"
#include <deque>
#include <vector>

namespace dalnim {
namespace {
    constexpr int kOpen = 0;
}

EventLog bfs(Grid grid, std::size_t start) {
    EventLog log;
    if (!grid_is_well_formed(grid) || grid.width == 0 || start >= grid.cells.size()) {
        return log;
    }
    if (grid.cells[start] != kOpen) {
        return log;
    }

    std::vector<bool> seen(grid.cells.size(), false);
    std::deque<std::size_t> queue;

    seen[start] = true;
    queue.push_back(start);
    log.push_back(Mark{.index = start, .kind = MarkKind::Frontier});

    while (!queue.empty()) {
        const std::size_t i = queue.front();
        queue.pop_front();

        log.push_back(Highlight{.index = i});
        log.push_back(Unmark{.index = i, .kind = MarkKind::Frontier});
        log.push_back(Mark{.index = i, .kind = MarkKind::Visited});

        for (std::size_t j : neighbours(grid, i)) {
            if (seen[j] || grid.cells[j] != kOpen) {
                continue;
            }
            seen[j] = true;
            queue.push_back(j);
            log.push_back(Mark{.index = j, .kind = MarkKind::Frontier});
        }
    }

    return log;
}
}
