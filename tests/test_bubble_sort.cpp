#include <doctest/doctest.h>
#include <algorithm>
#include <utility>
#include <vector>
#include "core/algos/bubble_sort.hpp"

namespace {
    // Replays a log against the state it was recorded from. If this does not
    // reproduce the sorted array, the log is not a faithful record of the run.
    std::vector<int> replay(std::vector<int> data, const dalnim::EventLog& log) {
        for (const dalnim::Event& e : log) {
            if (const auto* s = std::get_if<dalnim::Swap>(&e)) {
                std::swap(data[s->a], data[s->b]);
            }
        }
        return data;
    }

    std::size_t count_swaps(const dalnim::EventLog& log) {
        return std::count_if(log.begin(), log.end(), [](const dalnim::Event& e) {
            return std::holds_alternative<dalnim::Swap>(e);
        });
    }
}

TEST_CASE("an empty array produces no events") {
    CHECK(dalnim::bubble_sort({}).empty());
}

TEST_CASE("a one-element array produces no events") {
    CHECK(dalnim::bubble_sort({7}).empty());
}

TEST_CASE("an already sorted array compares but never swaps") {
    dalnim::EventLog log = dalnim::bubble_sort({1, 2, 3});
    CHECK(log.size() > 0);
    CHECK(count_swaps(log) == 0);
}

TEST_CASE("two out-of-order elements produce one compare and one swap") {
    dalnim::EventLog log = dalnim::bubble_sort({2, 1});
    REQUIRE(log.size() == 2);
    CHECK(std::holds_alternative<dalnim::Compare>(log[0]));
    CHECK(std::holds_alternative<dalnim::Swap>(log[1]));
}

TEST_CASE("swaps always name adjacent indices") {
    dalnim::EventLog log = dalnim::bubble_sort({5, 3, 8, 1, 9, 2});
    for (const dalnim::Event& e : log) {
        if (const auto* s = std::get_if<dalnim::Swap>(&e)) {
            CHECK(s->b == s->a + 1);
        }
    }
}

TEST_CASE("replaying the log reproduces the sorted array") {
    std::vector<int> input{5, 3, 8, 1, 9, 2};
    dalnim::EventLog log = dalnim::bubble_sort(input);

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());

    CHECK(replay(input, log) == expected);
}

TEST_CASE("replaying a prefix of the log reproduces that step exactly") {
    std::vector<int> input{3, 1, 2};
    dalnim::EventLog log = dalnim::bubble_sort(input);

    dalnim::EventLog prefix(log.begin(), log.begin() + 2);
    CHECK(replay(input, prefix) == std::vector<int>{1, 3, 2});
}
