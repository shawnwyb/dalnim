#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/flood_fill.hpp"
#include "core/grid_layout.hpp"
#include "core/layout.hpp"

using dalnim::kSecondsPerEvent;

namespace {
    dalnim::Grid make(std::size_t width, std::size_t height, std::vector<int> cells) {
        return dalnim::Grid{.width = width, .height = height, .cells = std::move(cells)};
    }
}

TEST_CASE("duration is one slot per event") {
    auto grid = make(2, 2, {0, 0, 0, 0});
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 0, 1));
    CHECK(anim.duration == doctest::Approx(anim.log.size() * kSecondsPerEvent));
}

TEST_CASE("an empty log leaves the grid untouched forever") {
    auto grid = make(2, 2, {3, 3, 3, 3});
    auto anim = dalnim::build_grid_animation(grid, {});

    CHECK(anim.duration == doctest::Approx(0.0));
    CHECK(dalnim::grid_values_at(anim, 0.0) == grid.cells);
    CHECK(dalnim::grid_values_at(anim, 99.0) == grid.cells);
    CHECK(dalnim::highlighted_at(anim, 0.0).has_value() == false);
}

TEST_CASE("at time zero nothing has been written yet") {
    auto grid = make(2, 2, {0, 0, 0, 0});
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 0, 1));
    CHECK(dalnim::grid_values_at(anim, 0.0) == grid.cells);
}

TEST_CASE("at the end every write has landed") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 4, 1));
    CHECK(dalnim::grid_values_at(anim, anim.duration) == std::vector<int>(9, 1));
}

TEST_CASE("past the end the grid holds its final state") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 4, 1));
    CHECK(dalnim::grid_values_at(anim, 500.0) == std::vector<int>(9, 1));
}

TEST_CASE("a write lands at the end of its own slot") {
    auto grid = make(1, 1, {0});
    // flood fill of one cell records exactly: highlight, then set
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 0, 1));
    REQUIRE(anim.log.size() == 2);

    CHECK(dalnim::grid_values_at(anim, 1.5 * kSecondsPerEvent) == std::vector<int>{0});
    CHECK(dalnim::grid_values_at(anim, 2.0 * kSecondsPerEvent) == std::vector<int>{1});
}

TEST_CASE("the highlight lasts exactly its own slot") {
    auto grid = make(1, 1, {0});
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 0, 1));

    CHECK(dalnim::highlighted_at(anim, 0.0) == 0);
    CHECK(dalnim::highlighted_at(anim, 0.5 * kSecondsPerEvent) == 0);
    CHECK(dalnim::highlighted_at(anim, 1.0 * kSecondsPerEvent).has_value() == false);
}

TEST_CASE("scrubbing backwards gives the same answer as playing forwards") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 0, 1));

    std::vector<double> times;
    for (double t = 0.0; t <= anim.duration; t += kSecondsPerEvent / 3.0) {
        times.push_back(t);
    }

    std::vector<std::vector<int>> forwards;
    for (double t : times) {
        forwards.push_back(dalnim::grid_values_at(anim, t));
    }

    for (std::size_t i = times.size(); i > 0; --i) {
        CHECK(dalnim::grid_values_at(anim, times[i - 1]) == forwards[i - 1]);
    }
}

TEST_CASE("every step writes at most one more cell than the step before") {
    auto grid = make(3, 3, std::vector<int>(9, 0));
    auto anim = dalnim::build_grid_animation(grid, dalnim::flood_fill(grid, 4, 1));

    std::size_t previous = 0;
    for (double t = 0.0; t <= anim.duration; t += kSecondsPerEvent) {
        const std::vector<int> cells = dalnim::grid_values_at(anim, t);
        const auto filled = static_cast<std::size_t>(std::count(cells.begin(), cells.end(), 1));
        CHECK(filled >= previous);
        CHECK(filled <= previous + 1);
        previous = filled;
    }
}
