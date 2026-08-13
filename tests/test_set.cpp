#include <doctest/doctest.h>
#include <algorithm>
#include <vector>
#include "core/algos/has_duplicate.hpp"
#include "core/set_layout.hpp"

namespace {
    // What the algorithm's own set holds, worked out only from Push.
    std::vector<int> inserted(const dalnim::EventLog& log) {
        std::vector<int> members;
        for (const dalnim::Event& e : log) {
            if (const auto* push = std::get_if<dalnim::Push>(&e)) {
                members.push_back(push->value);
            }
        }
        return members;
    }

    // The algorithm returns true exactly when a lookup hits, and a hit is the only
    // thing that records a Compare.
    bool answered_true(const dalnim::EventLog& log) {
        return std::any_of(log.begin(), log.end(), [](const dalnim::Event& e) {
            return std::holds_alternative<dalnim::Compare>(e);
        });
    }

    const dalnim::Compare* first_compare(const dalnim::EventLog& log) {
        for (const dalnim::Event& e : log) {
            if (const auto* c = std::get_if<dalnim::Compare>(&e)) {
                return c;
            }
        }
        return nullptr;
    }
}

TEST_CASE("an empty input produces no events") {
    CHECK(dalnim::has_duplicate({}).empty());
}

TEST_CASE("distinct values all reach the set and none of them matches") {
    const dalnim::EventLog log = dalnim::has_duplicate({5, 3, 8, 1});
    CHECK(inserted(log) == std::vector<int>{5, 3, 8, 1});
    CHECK(answered_true(log) == false);
}

TEST_CASE("a repeated value stops the walk where it is found") {
    const dalnim::EventLog log = dalnim::has_duplicate({5, 3, 5, 9});
    CHECK(answered_true(log));

    // 9 sits past the answer, so it is never looked at and never inserted.
    CHECK(inserted(log) == std::vector<int>{5, 3});
}

TEST_CASE("the match names the slot the value first came from") {
    const dalnim::EventLog log = dalnim::has_duplicate({5, 3, 5});
    const dalnim::Compare* match = first_compare(log);
    REQUIRE(match != nullptr);
    CHECK(match->a == 0);
    CHECK(match->b == 2);
}

TEST_CASE("the earliest duplicate wins when there are several") {
    const dalnim::EventLog log = dalnim::has_duplicate({4, 7, 7, 4});
    const dalnim::Compare* match = first_compare(log);
    REQUIRE(match != nullptr);
    CHECK(match->a == 1);
    CHECK(match->b == 2);
}

TEST_CASE("neighbouring duplicates are caught too") {
    CHECK(answered_true(dalnim::has_duplicate({7, 7})));
}

TEST_CASE("a single value cannot be a duplicate of itself") {
    CHECK(answered_true(dalnim::has_duplicate({7})) == false);
}

// A set only ever grows here. If a Pop ever appears, the pile the view draws would
// start shrinking, which is not something this algorithm can do.
TEST_CASE("nothing is ever taken back out of the set") {
    for (const std::vector<int>& input : {std::vector<int>{},
                                          std::vector<int>{7},
                                          std::vector<int>{5, 3, 8, 1},
                                          std::vector<int>{4, 7, 7, 4},
                                          std::vector<int>{-2, 0, -2}}) {
        const dalnim::EventLog log = dalnim::has_duplicate(input);
        CHECK(std::none_of(log.begin(), log.end(), [](const dalnim::Event& e) {
            return std::holds_alternative<dalnim::Pop>(e);
        }));
    }
}

TEST_CASE("the set fills up as the walk goes on") {
    const std::vector<int> input{5, 3, 8};
    const dalnim::SetAnimation anim =
        dalnim::build_set_animation(input, dalnim::has_duplicate(input));

    CHECK(dalnim::set_members_at(anim, 0.0).empty());
    CHECK(dalnim::set_members_at(anim, anim.duration) == input);
}

TEST_CASE("both slots of the answer are marked, and nothing else is") {
    const std::vector<int> input{5, 3, 5};
    const dalnim::SetAnimation anim =
        dalnim::build_set_animation(input, dalnim::has_duplicate(input));

    const std::vector<std::optional<dalnim::MarkKind>> marks =
        dalnim::set_marks_at(anim, anim.duration);
    REQUIRE(marks.size() == 3);
    CHECK(marks[0] == dalnim::MarkKind::Answer);
    CHECK(marks[1].has_value() == false);
    CHECK(marks[2] == dalnim::MarkKind::Answer);
}

TEST_CASE("the match stays drawn once it has happened") {
    const std::vector<int> input{5, 3, 5};
    const dalnim::SetAnimation anim =
        dalnim::build_set_animation(input, dalnim::has_duplicate(input));

    CHECK(dalnim::set_compare_at(anim, 0.0).has_value() == false);

    const auto matched = dalnim::set_compare_at(anim, anim.duration);
    REQUIRE(matched.has_value());
    CHECK(matched->first == 0);
    CHECK(matched->second == 2);
}
