#include <doctest/doctest.h>
#include <algorithm>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "core/algos/registry.hpp"

namespace {
    std::vector<int> replay(std::vector<int> data, const dalnim::EventLog& log) {
        for (const dalnim::Event& e : log) {
            if (const auto* s = std::get_if<dalnim::Swap>(&e)) {
                std::swap(data[s->a], data[s->b]);
            }
        }
        return data;
    }
}

TEST_CASE("the registry is not empty") {
    CHECK(dalnim::algorithms().size() >= 2);
}

TEST_CASE("the registry holds both array and grid algorithms") {
    bool any_array = false;
    bool any_grid = false;
    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        (dalnim::wants_array(algo) ? any_array : any_grid) = true;
    }
    CHECK(any_array);
    CHECK(any_grid);
}

TEST_CASE("every grid algorithm's log replays to a fully explored region") {
    const dalnim::Grid grid{.width = 3, .height = 3, .cells = std::vector<int>(9, 0)};
    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        // Asking for the pointer, rather than for anything-that-is-not-an-array,
        // stays correct as more kinds of algorithm arrive.
        const auto* run = std::get_if<dalnim::GridAlgorithm>(&algo.run);
        if (run == nullptr) {
            continue;
        }
        const std::string name = algo.name;
        CAPTURE(name);
        const dalnim::EventLog log = (*run)(grid, 0);
        CHECK(log.empty() == false);
        for (const dalnim::Event& e : log) {
            if (const auto* s = std::get_if<dalnim::Set>(&e)) {
                CHECK(s->index < grid.cells.size());
            }
        }
    }
}

TEST_CASE("every algorithm has a name") {
    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        REQUIRE(algo.name != nullptr);
        CHECK(std::string(algo.name).empty() == false);
    }
}

TEST_CASE("no two algorithms share a name") {
    std::vector<std::string> names;
    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        names.emplace_back(algo.name);
    }
    std::sort(names.begin(), names.end());
    CHECK(std::adjacent_find(names.begin(), names.end()) == names.end());
}

// Bars is the view for sorting, so it stands in for "this algorithm sorts". The
// registry does not record that claim directly; if a non-sorting algorithm ever
// wants bars, this needs a real flag rather than a proxy.
TEST_CASE("every bar-view algorithm's log replays to a sorted array") {
    const std::vector<std::vector<int>> inputs{
        {},
        {7},
        {2, 1},
        {1, 2, 3},
        {3, 2, 1},
        {5, 3, 8, 1, 9, 2},
        {4, 4, 4, 4},
        {-3, 10, -7, 0, 2},
    };

    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        if (algo.view != dalnim::View::Bars) {
            continue;
        }
        const std::string name = algo.name;
        CAPTURE(name);
        const auto run = std::get<dalnim::ArrayAlgorithm>(algo.run);
        for (const std::vector<int>& input : inputs) {
            std::vector<int> expected = input;
            std::sort(expected.begin(), expected.end());
            CHECK(replay(input, run(input)) == expected);
        }
    }
}

TEST_CASE("every algorithm names indices inside the array") {
    const std::vector<int> input{5, 3, 8, 1, 9, 2};

    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        const auto* run = std::get_if<dalnim::ArrayAlgorithm>(&algo.run);
        if (run == nullptr) {
            continue;
        }
        const std::string name = algo.name;
        CAPTURE(name);
        for (const dalnim::Event& e : (*run)(input)) {
            std::visit([&](const auto& ev) {
                using Kind = std::decay_t<decltype(ev)>;
                // Whatever an event names, it has to be a real slot in the array.
                if constexpr (requires { ev.a; ev.b; }) {
                    CHECK(ev.a < input.size());
                    CHECK(ev.b < input.size());
                } else if constexpr (requires { ev.index; }) {
                    CHECK(ev.index < input.size());
                }
            }, e);
        }
    }
}
