#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/bfs.hpp"
#include "core/grid_layout.hpp"

namespace {
    dalnim::Grid make(std::size_t width, std::size_t height, std::vector<int> cells) {
        return dalnim::Grid{.width = width, .height = height, .cells = std::move(cells)};
    }

    std::size_t count_visits(const dalnim::EventLog& log) {
        return std::count_if(log.begin(), log.end(), [](const dalnim::Event& e) {
            const auto* m = std::get_if<dalnim::Mark>(&e);
            return m != nullptr && m->kind == dalnim::MarkKind::Visited;
        });
    }
}

TEST_CASE("an empty grid produces no events") {
    CHECK(dalnim::bfs(make(0, 0, {}), 0).empty());
}

TEST_CASE("starting on a wall produces no events") {
    CHECK(dalnim::bfs(make(2, 2, {9, 0, 0, 0}), 0).empty());
}

TEST_CASE("every open cell is visited exactly once") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    CHECK(count_visits(dalnim::bfs(grid, 0)) == 9);
}

TEST_CASE("walls are never visited") {
    auto grid = make(3, 3, {0, 9, 0,
                            0, 9, 0,
                            0, 9, 0});
    auto log = dalnim::bfs(grid, 0);
    CHECK(count_visits(log) == 3);

    for (const dalnim::Event& e : log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            CHECK(grid.cells[m->index] == 0);
        }
    }
}

TEST_CASE("a cell is queued before it is visited") {
    auto grid = make(2, 2, std::vector<int>(4, 0));
    auto log = dalnim::bfs(grid, 0);

    std::vector<bool> queued(4, false);
    for (const dalnim::Event& e : log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            if (m->kind == dalnim::MarkKind::Frontier) {
                queued[m->index] = true;
            } else if (m->kind == dalnim::MarkKind::Visited) {
                CHECK(queued[m->index] == true);
            }
        }
    }
}

TEST_CASE("cells are visited in order of distance from the start") {
    // one row, so distance is simply how far along the row a cell sits
    auto grid = make(5, 1, std::vector<int>(5, 0));
    auto log = dalnim::bfs(grid, 0);

    std::vector<std::size_t> order;
    for (const dalnim::Event& e : log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            if (m->kind == dalnim::MarkKind::Visited) {
                order.push_back(m->index);
            }
        }
    }
    CHECK(order == std::vector<std::size_t>{0, 1, 2, 3, 4});
}

TEST_CASE("at the end every reachable cell is marked visited") {
    auto grid = make(3, 3, {0, 0, 9,
                            0, 9, 9,
                            0, 0, 0});
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));
    const auto marks = dalnim::marks_at(anim, anim.duration);

    for (std::size_t i = 0; i < grid.cells.size(); ++i) {
        CAPTURE(i);
        if (grid.cells[i] == 0) {
            CHECK(marks[i] == dalnim::MarkKind::Visited);
        } else {
            CHECK(marks[i].has_value() == false);
        }
    }
}

TEST_CASE("nothing is marked before the run begins") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    for (const auto& mark : dalnim::marks_at(anim, 0.0)) {
        CHECK(mark.has_value() == false);
    }
}

TEST_CASE("a cell is never both queued and visited at the same moment") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        const auto marks = dalnim::marks_at(anim, t);
        for (const auto& mark : marks) {
            if (mark.has_value()) {
                CHECK((*mark == dalnim::MarkKind::Frontier ||
                       *mark == dalnim::MarkKind::Visited));
            }
        }
    }
}

TEST_CASE("an unmark only clears the kind it names") {
    dalnim::Grid grid = make(1, 1, {0});
    dalnim::EventLog log{
        dalnim::Mark{.index = 0, .kind = dalnim::MarkKind::Visited},
        dalnim::Unmark{.index = 0, .kind = dalnim::MarkKind::Frontier},
    };
    auto anim = dalnim::build_grid_animation(grid, log);
    CHECK(dalnim::marks_at(anim, anim.duration)[0] == dalnim::MarkKind::Visited);
}

TEST_CASE("mark events describe themselves") {
    CHECK(dalnim::describe(dalnim::Mark{.index = 3, .kind = dalnim::MarkKind::Visited}) ==
          "marking 3 visited");
    CHECK(dalnim::describe(dalnim::Unmark{.index = 3, .kind = dalnim::MarkKind::Frontier}) ==
          "clearing frontier from 3");
}

TEST_CASE("the queue is empty before the run and after it") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    CHECK(dalnim::frontier_at(anim, 0.0).empty());
    CHECK(dalnim::frontier_at(anim, anim.duration).empty());
}

TEST_CASE("a cell joins the queue before it is taken off") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    // the start is queued by the first event, so it is waiting right after it
    CHECK(dalnim::frontier_at(anim, 1.0) == std::vector<std::size_t>{0});
}

TEST_CASE("the queue keeps the order cells were added in") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    std::vector<std::size_t> seen_leaving;
    std::vector<std::size_t> previous;
    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        const std::vector<std::size_t> now = dalnim::frontier_at(anim, t);
        // whatever left the queue since the last look must have been at its front
        if (!previous.empty() && (now.empty() || now.front() != previous.front())) {
            seen_leaving.push_back(previous.front());
        }
        previous = now;
    }

    // BFS takes cells in distance order, so the departures are the visit order
    std::vector<std::size_t> visits;
    for (const dalnim::Event& e : anim.log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            if (m->kind == dalnim::MarkKind::Visited) {
                visits.push_back(m->index);
            }
        }
    }
    CHECK(seen_leaving.size() > 0);
    for (std::size_t i = 0; i < seen_leaving.size(); ++i) {
        CHECK(seen_leaving[i] == visits[i]);
    }
}

TEST_CASE("no cell is in the queue twice") {
    auto grid = make(4, 4, std::vector<int>(16, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        std::vector<std::size_t> waiting = dalnim::frontier_at(anim, t);
        std::sort(waiting.begin(), waiting.end());
        CHECK(std::adjacent_find(waiting.begin(), waiting.end()) == waiting.end());
    }
}

TEST_CASE("the queue never holds a cell already visited") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::bfs(grid, 0));

    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        const auto marks = dalnim::marks_at(anim, t);
        for (std::size_t cell : dalnim::frontier_at(anim, t)) {
            CHECK(marks[cell] != dalnim::MarkKind::Visited);
        }
    }
}
