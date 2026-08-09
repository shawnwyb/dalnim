#pragma once
#include <cstddef>
#include <vector>

namespace dalnim {
    // A binary tree held positionally: slot i's children are slots 2i+1 and 2i+2,
    // whether or not they hold anything. Events name a slot, exactly as on a grid.
    struct Tree {
        std::vector<int> values;
        std::vector<bool> present;
    };

    std::size_t left_child(std::size_t slot);
    std::size_t right_child(std::size_t slot);
    std::size_t parent_of(std::size_t slot);

    // How far below the root a slot sits; the root is depth zero.
    std::size_t depth_of(std::size_t slot);

    bool has_node(const Tree& tree, std::size_t slot);
    std::size_t node_count(const Tree& tree);
    std::size_t tree_depth(const Tree& tree);
}
