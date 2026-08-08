#include <doctest/doctest.h>
#include <algorithm>
#include <string>
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

TEST_CASE("every algorithm's log replays to a sorted array") {
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
        CAPTURE(algo.name);
        for (const std::vector<int>& input : inputs) {
            std::vector<int> expected = input;
            std::sort(expected.begin(), expected.end());
            CHECK(replay(input, algo.run(input)) == expected);
        }
    }
}

TEST_CASE("every algorithm names indices inside the array") {
    const std::vector<int> input{5, 3, 8, 1, 9, 2};

    for (const dalnim::Algorithm& algo : dalnim::algorithms()) {
        CAPTURE(algo.name);
        for (const dalnim::Event& e : algo.run(input)) {
            std::visit([&](const auto& ev) {
                CHECK(ev.a < input.size());
                CHECK(ev.b < input.size());
            }, e);
        }
    }
}
