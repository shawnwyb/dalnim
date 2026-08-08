#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/flood_fill.hpp"

namespace {
    dalnim::Grid make(std::size_t width, std::size_t height, std::vector<int> cells) {
        return dalnim::Grid{.width = width, .height = height, .cells = std::move(cells)};
    }

    std::vector<int> replay(dalnim::Grid grid, const dalnim::EventLog& log) {
        for (const dalnim::Event& e : log) {
            if (const auto* s = std::get_if<dalnim::Set>(&e)) {
                grid.cells[s->index] = s->value;
            }
        }
        return grid.cells;
    }

    std::size_t count_sets(const dalnim::EventLog& log) {
        return std::count_if(log.begin(), log.end(), [](const dalnim::Event& e) {
            return std::holds_alternative<dalnim::Set>(e);
        });
    }
}

TEST_CASE("an empty grid produces no events") {
    CHECK(dalnim::flood_fill(make(0, 0, {}), 0, 1).empty());
}

TEST_CASE("a start outside the grid produces no events") {
    CHECK(dalnim::flood_fill(make(2, 2, {0, 0, 0, 0}), 9, 1).empty());
}

TEST_CASE("filling with the value already there produces no events") {
    CHECK(dalnim::flood_fill(make(2, 2, {5, 5, 5, 5}), 0, 5).empty());
}

TEST_CASE("a mismatched cell count produces no events") {
    CHECK(dalnim::flood_fill(make(3, 3, {0, 0}), 0, 1).empty());
}

TEST_CASE("one cell is highlighted then set") {
    auto log = dalnim::flood_fill(make(1, 1, {0}), 0, 1);
    REQUIRE(log.size() == 2);
    CHECK(std::holds_alternative<dalnim::Highlight>(log[0]));
    CHECK(std::get<dalnim::Set>(log[1]).value == 1);
}

TEST_CASE("a uniform grid is filled entirely") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto log = dalnim::flood_fill(grid, 4, 1);
    CHECK(count_sets(log) == 9);
    CHECK(replay(grid, log) == std::vector<int>(9, 1));
}

TEST_CASE("a wall stops the fill") {
    // column 1 is a wall of 9s, so filling from the left never reaches the right
    auto grid = make(3, 3, {0, 9, 0,
                            0, 9, 0,
                            0, 9, 0});
    auto log = dalnim::flood_fill(grid, 0, 1);
    CHECK(count_sets(log) == 3);
    CHECK(replay(grid, log) == std::vector<int>{1, 9, 0,
                                                1, 9, 0,
                                                1, 9, 0});
}

TEST_CASE("diagonals are not connected") {
    auto grid = make(2, 2, {0, 9,
                            9, 0});
    auto log = dalnim::flood_fill(grid, 0, 1);
    CHECK(count_sets(log) == 1);
    CHECK(replay(grid, log) == std::vector<int>{1, 9, 9, 0});
}

TEST_CASE("no cell is filled twice") {
    auto grid = make(4, 4, std::vector<int>(16, 0));
    auto log = dalnim::flood_fill(grid, 0, 1);

    std::vector<bool> seen(16, false);
    for (const dalnim::Event& e : log) {
        if (const auto* s = std::get_if<dalnim::Set>(&e)) {
            CHECK(seen[s->index] == false);
            seen[s->index] = true;
        }
    }
}

TEST_CASE("every event names a cell inside the grid") {
    auto grid = make(4, 3, std::vector<int>(12, 0));
    for (const dalnim::Event& e : dalnim::flood_fill(grid, 5, 1)) {
        std::visit([&](const auto& ev) {
            using Kind = std::decay_t<decltype(ev)>;
            if constexpr (std::is_same_v<Kind, dalnim::Highlight> ||
                          std::is_same_v<Kind, dalnim::Set>) {
                CHECK(ev.index < grid.cells.size());
            }
        }, e);
    }
}

TEST_CASE("cell_index walks row by row") {
    auto grid = make(4, 3, std::vector<int>(12, 0));
    CHECK(dalnim::cell_index(grid, 0, 0) == 0);
    CHECK(dalnim::cell_index(grid, 0, 3) == 3);
    CHECK(dalnim::cell_index(grid, 1, 0) == 4);
    CHECK(dalnim::cell_index(grid, 2, 3) == 11);
}
