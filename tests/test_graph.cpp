#include <doctest/doctest.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include "core/algos/graph_bfs.hpp"
#include "core/graph_layout.hpp"
#include "core/parse.hpp"

using dalnim::parse_graph;

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

TEST_CASE("an empty line still counts as a node") {
    const dalnim::Graph graph = parse_graph("1\n0\n");
    CHECK(dalnim::node_count(graph) == 2);
}

TEST_CASE("an edge written once is stored at both ends") {
    const dalnim::Graph graph = parse_graph("1\n\n");
    CHECK(dalnim::has_edge(graph, 0, 1));
    CHECK(dalnim::has_edge(graph, 1, 0));
    CHECK(dalnim::edge_count(graph) == 1);
}

TEST_CASE("writing an edge from both ends does not double it") {
    const dalnim::Graph graph = parse_graph("1\n0\n");
    CHECK(dalnim::edge_count(graph) == 1);
    CHECK(graph.neighbours[0].size() == 1);
}

TEST_CASE("a node reached only by being mentioned still exists") {
    // node 3 has no line of its own, but node 0 names it
    const dalnim::Graph graph = parse_graph("3\n\n");
    CHECK(dalnim::node_count(graph) == 4);
    CHECK(dalnim::has_edge(graph, 3, 0));
}

TEST_CASE("an edge to itself, or to a negative, is dropped") {
    const dalnim::Graph graph = parse_graph("0 -3 1\n0\n");
    CHECK(dalnim::has_edge(graph, 0, 0) == false);
    CHECK(dalnim::edge_count(graph) == 1);
}

TEST_CASE("blank text is a graph with nothing in it") {
    CHECK(dalnim::node_count(parse_graph("")) == 0);
    CHECK(dalnim::node_count(parse_graph("   \n\n")) == 0);
}

TEST_CASE("an empty graph produces no events") {
    CHECK(dalnim::graph_bfs(parse_graph(""), 0).empty());
}

TEST_CASE("a start outside the graph produces no events") {
    CHECK(dalnim::graph_bfs(parse_graph("1\n0\n"), 7).empty());
}

TEST_CASE("every reachable node is visited exactly once") {
    const dalnim::Graph graph = parse_graph("1 2\n0 3 4\n0 5\n1\n1 5\n2 4");
    const std::vector<std::size_t> order = visit_order(dalnim::graph_bfs(graph, 0));

    CHECK(order.size() == 6);
    std::vector<std::size_t> sorted = order;
    std::sort(sorted.begin(), sorted.end());
    CHECK(std::adjacent_find(sorted.begin(), sorted.end()) == sorted.end());
}

TEST_CASE("nodes in another component are never reached") {
    // 0-1 apart from 2-3
    const dalnim::Graph graph = parse_graph("1\n0\n3\n2");
    const std::vector<std::size_t> order = visit_order(dalnim::graph_bfs(graph, 0));
    CHECK(order == std::vector<std::size_t>{0, 1});
}

TEST_CASE("nodes are reached in order of how many edges away they are") {
    // a straight chain, so distance is the node number
    const dalnim::Graph graph = parse_graph("1\n2\n3\n4\n");
    CHECK(visit_order(dalnim::graph_bfs(graph, 0)) ==
          std::vector<std::size_t>{0, 1, 2, 3, 4});
}

TEST_CASE("a cycle does not send the walk round forever") {
    const dalnim::Graph graph = parse_graph("1 2\n2\n0");
    CHECK(visit_order(dalnim::graph_bfs(graph, 0)).size() == 3);
}

TEST_CASE("at the end every reached node is marked visited") {
    const dalnim::Graph graph = parse_graph("1 2\n0 3 4\n0 5\n1\n1 5\n2 4");
    const auto anim = dalnim::build_graph_animation(graph, dalnim::graph_bfs(graph, 0));
    const auto marks = dalnim::graph_marks_at(anim, anim.duration);

    for (std::size_t node = 0; node < dalnim::node_count(graph); ++node) {
        CAPTURE(node);
        CHECK(marks[node] == dalnim::MarkKind::Visited);
    }
}

TEST_CASE("the queue empties by the end") {
    const dalnim::Graph graph = parse_graph("1 2\n0 3 4\n0 5\n1\n1 5\n2 4");
    const auto anim = dalnim::build_graph_animation(graph, dalnim::graph_bfs(graph, 0));

    CHECK(dalnim::graph_frontier_at(anim, 0.0).empty());
    CHECK(dalnim::graph_frontier_at(anim, anim.duration).empty());
}

TEST_CASE("every node lands on the ring, none in the middle") {
    const dalnim::Graph graph = parse_graph("1 2\n0 3 4\n0 5\n1\n1 5\n2 4");
    const auto anim = dalnim::build_graph_animation(graph, dalnim::graph_bfs(graph, 0));

    const double centre = anim.span / 2.0;
    double first = 0.0;
    for (std::size_t node = 0; node < anim.positions.size(); ++node) {
        const double dx = anim.positions[node].x - centre;
        const double dy = anim.positions[node].y - centre;
        const double away = std::sqrt(dx * dx + dy * dy);
        if (node == 0) {
            first = away;
        }
        CAPTURE(node);
        CHECK(away == doctest::Approx(first).epsilon(0.001));
    }
}

TEST_CASE("no two nodes land on the same spot") {
    const dalnim::Graph graph = parse_graph("1\n2\n3\n4\n0");
    const auto anim = dalnim::build_graph_animation(graph, dalnim::graph_bfs(graph, 0));

    for (std::size_t a = 0; a < anim.positions.size(); ++a) {
        for (std::size_t b = a + 1; b < anim.positions.size(); ++b) {
            const double dx = anim.positions[a].x - anim.positions[b].x;
            const double dy = anim.positions[a].y - anim.positions[b].y;
            CHECK(std::sqrt(dx * dx + dy * dy) > 1.0);
        }
    }
}

TEST_CASE("more nodes means a wider ring") {
    const auto small = dalnim::build_graph_animation(parse_graph("1\n0"), {});
    const auto big = dalnim::build_graph_animation(
        parse_graph("1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n0"), {});
    CHECK(big.span > small.span);
}
