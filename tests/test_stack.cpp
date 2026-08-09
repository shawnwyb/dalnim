#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/monotonic_stack.hpp"
#include "core/stack_layout.hpp"

namespace {
    // What the algorithm's own stack holds, worked out only from Push and Pop.
    std::vector<int> replay(const dalnim::EventLog& log) {
        std::vector<int> pile;
        for (const dalnim::Event& e : log) {
            if (const auto* push = std::get_if<dalnim::Push>(&e)) {
                pile.push_back(push->value);
            } else if (std::holds_alternative<dalnim::Pop>(e) && !pile.empty()) {
                pile.pop_back();
            }
        }
        return pile;
    }
}

TEST_CASE("an empty input produces no events") {
    CHECK(dalnim::next_greater({}).empty());
}

TEST_CASE("one value is pushed and never popped") {
    auto log = dalnim::next_greater({7});
    CHECK(replay(log) == std::vector<int>{7});
}

TEST_CASE("a descending input never pops") {
    auto log = dalnim::next_greater({5, 4, 3});
    CHECK(replay(log) == std::vector<int>{5, 4, 3});
    CHECK(std::none_of(log.begin(), log.end(), [](const dalnim::Event& e) {
        return std::holds_alternative<dalnim::Pop>(e);
    }));
}

TEST_CASE("a larger value pops everything smaller") {
    auto log = dalnim::next_greater({1, 2, 3});
    CHECK(replay(log) == std::vector<int>{3});
}

TEST_CASE("equal values are kept, not popped") {
    auto log = dalnim::next_greater({4, 4, 4});
    CHECK(replay(log) == std::vector<int>{4, 4, 4});
}

TEST_CASE("the pile is never popped when empty") {
    auto log = dalnim::next_greater({9, 1, 8, 2, 7});
    int depth = 0;
    for (const dalnim::Event& e : log) {
        if (std::holds_alternative<dalnim::Push>(e)) {
            ++depth;
        } else if (std::holds_alternative<dalnim::Pop>(e)) {
            --depth;
            CHECK(depth >= 0);
        }
    }
}

TEST_CASE("the pile never has a smaller value below a larger one") {
    const std::vector<int> input{5, 3, 8, 1, 9, 2};
    auto anim = dalnim::build_stack_animation(input, dalnim::next_greater(input));

    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        const std::vector<int> pile = dalnim::stack_at(anim, t);
        for (std::size_t i = 0; i + 1 < pile.size(); ++i) {
            CAPTURE(t);
            CHECK(pile[i] >= pile[i + 1]);
        }
    }
}

TEST_CASE("the pile at the end matches a straight replay") {
    const std::vector<int> input{5, 3, 8, 1, 9, 2};
    auto log = dalnim::next_greater(input);
    auto anim = dalnim::build_stack_animation(input, log);
    CHECK(dalnim::stack_at(anim, anim.duration) == replay(log));
}

TEST_CASE("nothing is on the pile before the run begins") {
    const std::vector<int> input{5, 3, 8};
    auto anim = dalnim::build_stack_animation(input, dalnim::next_greater(input));
    CHECK(dalnim::stack_at(anim, 0.0).empty());
}

TEST_CASE("the cursor sticks to a value until the next one") {
    const std::vector<int> input{5, 3};
    auto anim = dalnim::build_stack_animation(input, dalnim::next_greater(input));

    // event 0 is Highlight{0}, so the cursor holds there through the events after it
    CHECK(dalnim::cursor_at(anim, 0.0) == 0);
    CHECK(dalnim::cursor_at(anim, 1.5) == 0);
    CHECK(dalnim::cursor_at(anim, anim.duration) == 1);
}

TEST_CASE("the cursor only ever moves forwards") {
    const std::vector<int> input{5, 3, 8, 1, 9, 2};
    auto anim = dalnim::build_stack_animation(input, dalnim::next_greater(input));

    std::size_t previous = 0;
    for (double t = 0.0; t <= anim.duration; t += 0.5) {
        if (const auto cursor = dalnim::cursor_at(anim, t)) {
            CHECK(*cursor >= previous);
            previous = *cursor;
        }
    }
}

TEST_CASE("a comparison names two real slots and lasts one event") {
    const std::vector<int> input{1, 2};
    auto anim = dalnim::build_stack_animation(input, dalnim::next_greater(input));

    std::size_t slots_with_a_comparison = 0;
    for (std::size_t k = 0; k < anim.log.size(); ++k) {
        const auto pair = dalnim::comparing_at(anim, static_cast<double>(k) + 0.5);
        if (pair.has_value()) {
            ++slots_with_a_comparison;
            CHECK(pair->first < input.size());
            CHECK(pair->second < input.size());
        }
    }
    CHECK(slots_with_a_comparison > 0);
}
