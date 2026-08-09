#include <doctest/doctest.h>
#include <vector>
#include "core/algos/cycle_detect.hpp"
#include "core/list_layout.hpp"

namespace {
    dalnim::LinkedList make(std::vector<int> values, int loops_to) {
        dalnim::LinkedList list;
        list.values = std::move(values);
        if (loops_to >= 0) {
            list.cycle_to = static_cast<std::size_t>(loops_to);
        }
        return list;
    }

    // Where the last Compare left the two pointers.
    std::pair<std::size_t, std::size_t> final_pointers(const dalnim::EventLog& log) {
        std::pair<std::size_t, std::size_t> found{0, 0};
        for (const dalnim::Event& e : log) {
            if (const auto* c = std::get_if<dalnim::Compare>(&e)) {
                found = {c->a, c->b};
            }
        }
        return found;
    }
}

TEST_CASE("a straight list walks off the end") {
    const auto list = make({1, 2, 3}, -1);
    CHECK(dalnim::next_of(list, 0) == 1);
    CHECK(dalnim::next_of(list, 2).has_value() == false);
    CHECK(dalnim::has_cycle(list) == false);
}

TEST_CASE("a loop sends the last node back") {
    const auto list = make({1, 2, 3, 4}, 1);
    CHECK(dalnim::next_of(list, 3) == 1);
    CHECK(dalnim::has_cycle(list));
}

TEST_CASE("a loop target past the end is ignored") {
    const auto list = make({1, 2}, 9);
    CHECK(dalnim::has_cycle(list) == false);
    CHECK(dalnim::next_of(list, 1).has_value() == false);
}

TEST_CASE("an empty list produces no events") {
    CHECK(dalnim::detect_cycle(make({}, -1)).empty());
}

TEST_CASE("a list with no loop never reports an answer") {
    const auto log = dalnim::detect_cycle(make({1, 2, 3, 4, 5}, -1));
    for (const dalnim::Event& e : log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            CHECK(m->kind != dalnim::MarkKind::Answer);
        }
    }
}

TEST_CASE("a list with a loop reports an answer") {
    const auto log = dalnim::detect_cycle(make({3, 2, 0, -4, 7, 9}, 2));
    bool answered = false;
    for (const dalnim::Event& e : log) {
        if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
            answered = answered || m->kind == dalnim::MarkKind::Answer;
        }
    }
    CHECK(answered);
}

TEST_CASE("the pointers meet inside the loop, not before it") {
    const auto list = make({3, 2, 0, -4, 7, 9}, 2);
    const auto pointers = final_pointers(dalnim::detect_cycle(list));
    CHECK(pointers.first == pointers.second);
    CHECK(pointers.first >= 2);
}

TEST_CASE("a one-node loop onto itself is found") {
    const auto log = dalnim::detect_cycle(make({7}, 0));
    const auto pointers = final_pointers(log);
    CHECK(pointers.first == 0);
    CHECK(pointers.second == 0);
}

TEST_CASE("the pointers never leave the list") {
    const auto list = make({3, 2, 0, -4, 7, 9}, 1);
    for (const dalnim::Event& e : dalnim::detect_cycle(list)) {
        if (const auto* c = std::get_if<dalnim::Compare>(&e)) {
            CHECK(c->a < list.values.size());
            CHECK(c->b < list.values.size());
        }
    }
}

TEST_CASE("every walk terminates, loop or not") {
    for (int loops_to = -1; loops_to < 6; ++loops_to) {
        CAPTURE(loops_to);
        const auto log = dalnim::detect_cycle(make({3, 2, 0, -4, 7, 9}, loops_to));
        CHECK(log.size() < 200);
    }
}

TEST_CASE("the pointers hold their place between comparisons") {
    const auto list = make({3, 2, 0, -4, 7, 9}, 2);
    const auto anim = dalnim::build_list_animation(list, dalnim::detect_cycle(list));

    CHECK(dalnim::pointers_at(anim, 0.0).has_value());
    // event 1 is a Mark, so the pointers must still read from event 0
    CHECK(dalnim::pointers_at(anim, 1.5) == dalnim::pointers_at(anim, 0.5));
}

TEST_CASE("the span leaves room for every node") {
    const auto list = make({1, 2, 3}, -1);
    const auto anim = dalnim::build_list_animation(list, dalnim::detect_cycle(list));
    CHECK(anim.span_x == doctest::Approx(2.0 * dalnim::kListPitch + dalnim::kListNodeUnits));
}

TEST_CASE("nothing is marked before the walk begins") {
    const auto list = make({3, 2, 0, -4}, 1);
    const auto anim = dalnim::build_list_animation(list, dalnim::detect_cycle(list));
    for (const auto& mark : dalnim::list_marks_at(anim, 0.0)) {
        CHECK(mark.has_value() == false);
    }
}
