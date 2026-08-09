#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/tree_dfs.hpp"
#include "core/parse.hpp"
#include "core/tree_layout.hpp"

using dalnim::parse_tree;

namespace {
    std::vector<std::size_t> visit_order(const dalnim::EventLog& log) {
        std::vector<std::size_t> order;
        for (const dalnim::Event& e : log) {
            if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
                if (m->kind == dalnim::MarkKind::Visited) {
                    order.push_back(m->index);
                }
            }
        }
        return order;
    }
}

TEST_CASE("slot arithmetic walks the tree") {
    CHECK(dalnim::left_child(0) == 1);
    CHECK(dalnim::right_child(0) == 2);
    CHECK(dalnim::left_child(2) == 5);
    CHECK(dalnim::parent_of(5) == 2);
    CHECK(dalnim::parent_of(0) == 0);
}

TEST_CASE("depth counts the steps back to the root") {
    CHECK(dalnim::depth_of(0) == 0);
    CHECK(dalnim::depth_of(1) == 1);
    CHECK(dalnim::depth_of(2) == 1);
    CHECK(dalnim::depth_of(3) == 2);
    CHECK(dalnim::depth_of(6) == 2);
    CHECK(dalnim::depth_of(7) == 3);
}

TEST_CASE("an empty string parses to no tree") {
    const dalnim::Tree tree = parse_tree("");
    CHECK(tree.present.empty());
    CHECK(dalnim::node_count(tree) == 0);
}

TEST_CASE("dots mark empty slots") {
    const dalnim::Tree tree = parse_tree("1 . 3");
    CHECK(tree.present.size() == 3);
    CHECK(dalnim::has_node(tree, 0));
    CHECK(dalnim::has_node(tree, 1) == false);
    CHECK(dalnim::has_node(tree, 2));
    CHECK(tree.values[2] == 3);
}

TEST_CASE("any non-number marks an empty slot") {
    CHECK(dalnim::has_node(parse_tree("1 null 3"), 1) == false);
    CHECK(dalnim::has_node(parse_tree("1 x 3"), 1) == false);
}

TEST_CASE("trailing empty slots are dropped") {
    const dalnim::Tree tree = parse_tree("1 2 3 . . . .");
    CHECK(tree.present.size() == 3);
}

TEST_CASE("negatives are values, not empty slots") {
    const dalnim::Tree tree = parse_tree("-5 . 3");
    CHECK(dalnim::has_node(tree, 0));
    CHECK(tree.values[0] == -5);
}

TEST_CASE("an empty tree produces no events") {
    CHECK(dalnim::tree_dfs(parse_tree("")).empty());
    CHECK(dalnim::tree_dfs(parse_tree(". 2 3")).empty());
}

TEST_CASE("depth-first visits the left subtree before the right") {
    // 1 over 2 and 3, with 2 over 4 and 5
    const dalnim::Tree tree = parse_tree("1 2 3 4 5");
    CHECK(visit_order(dalnim::tree_dfs(tree)) == std::vector<std::size_t>{0, 1, 3, 4, 2});
}

TEST_CASE("every node is visited exactly once") {
    const dalnim::Tree tree = parse_tree("8 3 10 1 6 . 14 . . 4 7 . . 13 .");
    const std::vector<std::size_t> order = visit_order(dalnim::tree_dfs(tree));

    CHECK(order.size() == dalnim::node_count(tree));
    std::vector<std::size_t> sorted = order;
    std::sort(sorted.begin(), sorted.end());
    CHECK(std::adjacent_find(sorted.begin(), sorted.end()) == sorted.end());
}

TEST_CASE("empty slots are never visited") {
    const dalnim::Tree tree = parse_tree("1 . 3 . . 6 7");
    for (std::size_t slot : visit_order(dalnim::tree_dfs(tree))) {
        CAPTURE(slot);
        CHECK(dalnim::has_node(tree, slot));
    }
}

TEST_CASE("the pile is never popped when empty") {
    const dalnim::Tree tree = parse_tree("8 3 10 1 6 . 14");
    int depth = 0;
    for (const dalnim::Event& e : dalnim::tree_dfs(tree)) {
        if (std::holds_alternative<dalnim::Push>(e)) {
            ++depth;
        } else if (std::holds_alternative<dalnim::Pop>(e)) {
            --depth;
            CHECK(depth >= 0);
        }
    }
    CHECK(depth == 0);
}

TEST_CASE("a node sits above and between its children") {
    const dalnim::Tree tree = parse_tree("1 2 3");
    const auto anim = dalnim::build_tree_animation(tree, dalnim::tree_dfs(tree));

    const dalnim::Point root = anim.positions[0];
    const dalnim::Point left = anim.positions[1];
    const dalnim::Point right = anim.positions[2];

    CHECK(root.y < left.y);
    CHECK(left.y == doctest::Approx(right.y));
    CHECK(left.x < root.x);
    CHECK(root.x < right.x);
}

TEST_CASE("all nodes at one depth share a row") {
    const dalnim::Tree tree = parse_tree("8 3 10 1 6 12 14");
    const auto anim = dalnim::build_tree_animation(tree, dalnim::tree_dfs(tree));

    CHECK(anim.positions[1].y == doctest::Approx(anim.positions[2].y));
    CHECK(anim.positions[3].y == doctest::Approx(anim.positions[6].y));
    CHECK(anim.positions[1].y < anim.positions[3].y);
}

TEST_CASE("no two nodes land on the same spot") {
    const dalnim::Tree tree = parse_tree("8 3 10 1 6 12 14");
    const auto anim = dalnim::build_tree_animation(tree, dalnim::tree_dfs(tree));

    for (std::size_t a = 0; a < anim.positions.size(); ++a) {
        for (std::size_t b = a + 1; b < anim.positions.size(); ++b) {
            if (!dalnim::has_node(tree, a) || !dalnim::has_node(tree, b)) {
                continue;
            }
            CHECK((anim.positions[a].x != anim.positions[b].x ||
                   anim.positions[a].y != anim.positions[b].y));
        }
    }
}

TEST_CASE("nothing is marked before the walk begins") {
    const dalnim::Tree tree = parse_tree("8 3 10");
    const auto anim = dalnim::build_tree_animation(tree, dalnim::tree_dfs(tree));

    for (const auto& mark : dalnim::tree_marks_at(anim, 0.0)) {
        CHECK(mark.has_value() == false);
    }
}

TEST_CASE("at the end every node is marked visited") {
    const dalnim::Tree tree = parse_tree("8 3 10 1 6 . 14");
    const auto anim = dalnim::build_tree_animation(tree, dalnim::tree_dfs(tree));
    const auto marks = dalnim::tree_marks_at(anim, anim.duration);

    for (std::size_t slot = 0; slot < tree.present.size(); ++slot) {
        CAPTURE(slot);
        if (dalnim::has_node(tree, slot)) {
            CHECK(marks[slot] == dalnim::MarkKind::Visited);
        } else {
            CHECK(marks[slot].has_value() == false);
        }
    }
}
