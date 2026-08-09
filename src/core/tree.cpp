#include "core/tree.hpp"

namespace dalnim {
    std::size_t left_child(std::size_t slot) {
        return 2 * slot + 1;
    }

    std::size_t right_child(std::size_t slot) {
        return 2 * slot + 2;
    }

    std::size_t parent_of(std::size_t slot) {
        return slot == 0 ? 0 : (slot - 1) / 2;
    }

    std::size_t depth_of(std::size_t slot) {
        std::size_t depth = 0;
        while (slot > 0) {
            slot = (slot - 1) / 2;
            ++depth;
        }
        return depth;
    }

    bool has_node(const Tree& tree, std::size_t slot) {
        return slot < tree.present.size() && tree.present[slot];
    }

    std::size_t node_count(const Tree& tree) {
        std::size_t count = 0;
        for (std::size_t i = 0; i < tree.present.size(); ++i) {
            if (tree.present[i]) {
                ++count;
            }
        }
        return count;
    }

    std::size_t tree_depth(const Tree& tree) {
        std::size_t deepest = 0;
        for (std::size_t i = 0; i < tree.present.size(); ++i) {
            if (tree.present[i]) {
                const std::size_t depth = depth_of(i);
                deepest = depth > deepest ? depth : deepest;
            }
        }
        return deepest;
    }
}
