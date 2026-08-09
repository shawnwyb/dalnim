#include "core/algos/graph_bfs.hpp"
#include <deque>
#include <vector>

namespace dalnim {
    EventLog graph_bfs(Graph graph, std::size_t start) {
        EventLog log;
        const std::size_t count = node_count(graph);
        if (count == 0 || start >= count) {
            return log;
        }

        std::vector<bool> seen(count, false);
        std::deque<std::size_t> queue;

        seen[start] = true;
        queue.push_back(start);
        log.push_back(Mark{.index = start, .kind = MarkKind::Frontier});

        while (!queue.empty()) {
            const std::size_t node = queue.front();
            queue.pop_front();

            log.push_back(Highlight{.index = node});
            log.push_back(Unmark{.index = node, .kind = MarkKind::Frontier});
            log.push_back(Mark{.index = node, .kind = MarkKind::Visited});

            for (std::size_t next : graph.neighbours[node]) {
                if (next >= count || seen[next]) {
                    continue;
                }
                seen[next] = true;
                queue.push_back(next);
                log.push_back(Mark{.index = next, .kind = MarkKind::Frontier});
            }
        }

        return log;
    }
}
