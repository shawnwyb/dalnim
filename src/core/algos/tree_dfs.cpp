#include "core/algos/tree_dfs.hpp"
#include <cstddef>
#include <vector>

namespace dalnim {
    EventLog tree_dfs(Tree tree) {
        EventLog log;
        if (!has_node(tree, 0)) {
            return log;
        }

        std::vector<std::size_t> pending{0};
        log.push_back(Push{.value = tree.values[0]});

        while (!pending.empty()) {
            const std::size_t slot = pending.back();
            pending.pop_back();

            log.push_back(Pop{});
            log.push_back(Highlight{.index = slot});
            log.push_back(Mark{.index = slot, .kind = MarkKind::Visited});

            // Right goes on first so the left child comes back off the pile first.
            const std::size_t right = right_child(slot);
            const std::size_t left = left_child(slot);
            if (has_node(tree, right)) {
                pending.push_back(right);
                log.push_back(Push{.value = tree.values[right]});
            }
            if (has_node(tree, left)) {
                pending.push_back(left);
                log.push_back(Push{.value = tree.values[left]});
            }
        }

        return log;
    }
}
