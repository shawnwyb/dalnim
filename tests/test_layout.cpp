#include <doctest/doctest.h>
#include <vector>
#include "core/algos/bubble_sort.hpp"
#include "core/layout.hpp"

using dalnim::kBoxSpacing;
using dalnim::kSecondsPerEvent;

TEST_CASE("one timeline per box") {
    auto anim = dalnim::build_array_animation({5, 3, 8}, {});
    CHECK(anim.x.size() == 3);
}

TEST_CASE("with no events every box holds its starting spot") {
    auto anim = dalnim::build_array_animation({5, 3, 8}, {});
    CHECK(anim.duration == doctest::Approx(0.0));
    for (std::size_t i = 0; i < 3; ++i) {
        CHECK(anim.x[i].sample(0.0) == doctest::Approx(i * kBoxSpacing));
        CHECK(anim.x[i].sample(99.0) == doctest::Approx(i * kBoxSpacing));
    }
}

TEST_CASE("duration is one step per event, compares included") {
    dalnim::EventLog log{
        dalnim::Compare{.a=0, .b=1},
        dalnim::Swap{.a=0, .b=1},
    };
    auto anim = dalnim::build_array_animation({2, 1}, log);
    CHECK(anim.duration == doctest::Approx(2 * kSecondsPerEvent));
}

TEST_CASE("a swap slides both boxes into each other's slot") {
    dalnim::EventLog log{dalnim::Swap{.a=0, .b=1}};
    auto anim = dalnim::build_array_animation({2, 1}, log);

    CHECK(anim.x[0].sample(0.0) == doctest::Approx(0.0));
    CHECK(anim.x[1].sample(0.0) == doctest::Approx(kBoxSpacing));

    CHECK(anim.x[0].sample(kSecondsPerEvent) == doctest::Approx(kBoxSpacing));
    CHECK(anim.x[1].sample(kSecondsPerEvent) == doctest::Approx(0.0));
}

TEST_CASE("mid-swap both boxes sit halfway") {
    dalnim::EventLog log{dalnim::Swap{.a=0, .b=1}};
    auto anim = dalnim::build_array_animation({2, 1}, log);

    const double halfway = kBoxSpacing / 2.0;
    CHECK(anim.x[0].sample(kSecondsPerEvent / 2.0) == doctest::Approx(halfway));
    CHECK(anim.x[1].sample(kSecondsPerEvent / 2.0) == doctest::Approx(halfway));
}

TEST_CASE("a box moved twice ends in the right slot") {
    dalnim::EventLog log{
        dalnim::Swap{.a=0, .b=1},
        dalnim::Swap{.a=1, .b=2},
    };
    auto anim = dalnim::build_array_animation({3, 2, 1}, log);

    const double end = 2 * kSecondsPerEvent;
    CHECK(anim.x[0].sample(end) == doctest::Approx(2 * kBoxSpacing));
    CHECK(anim.x[1].sample(end) == doctest::Approx(0.0));
    CHECK(anim.x[2].sample(end) == doctest::Approx(kBoxSpacing));
}

TEST_CASE("a box waits in place until its own swap begins") {
    dalnim::EventLog log{
        dalnim::Swap{.a=0, .b=1},
        dalnim::Swap{.a=1, .b=2},
    };
    auto anim = dalnim::build_array_animation({3, 2, 1}, log);

    CHECK(anim.x[2].sample(kSecondsPerEvent) == doctest::Approx(2 * kBoxSpacing));
}

TEST_CASE("every box lands on a distinct slot at the end of a real sort") {
    std::vector<int> input{5, 3, 8, 1, 9, 2};
    auto anim = dalnim::build_array_animation(input, dalnim::bubble_sort(input));

    std::vector<bool> slot_taken(input.size(), false);
    for (const dalnim::Timeline& t : anim.x) {
        const double slot = t.sample(anim.duration) / kBoxSpacing;
        const auto index = static_cast<std::size_t>(slot + 0.5);
        REQUIRE(index < slot_taken.size());
        CHECK(slot_taken[index] == false);
        slot_taken[index] = true;
    }
}

TEST_CASE("a compare is recorded as a time span over two boxes") {
    dalnim::EventLog log{dalnim::Compare{.a=0, .b=1}};
    auto anim = dalnim::build_array_animation({2, 1}, log);

    REQUIRE(anim.compares.size() == 1);
    CHECK(anim.compares[0].begin == doctest::Approx(0.0));
    CHECK(anim.compares[0].end == doctest::Approx(kSecondsPerEvent));
    CHECK(anim.compares[0].box_a == 0);
    CHECK(anim.compares[0].box_b == 1);
}

TEST_CASE("nothing is compared outside a compare span") {
    dalnim::EventLog log{dalnim::Compare{.a=0, .b=1}};
    auto anim = dalnim::build_array_animation({2, 1}, log);

    CHECK(dalnim::compare_at(anim, kSecondsPerEvent / 2.0) != nullptr);
    CHECK(dalnim::compare_at(anim, kSecondsPerEvent) == nullptr);
    CHECK(dalnim::compare_at(anim, 99.0) == nullptr);
}

TEST_CASE("a compare after a swap names the boxes now in those slots") {
    dalnim::EventLog log{
        dalnim::Swap{.a=0, .b=1},
        dalnim::Compare{.a=0, .b=1},
    };
    auto anim = dalnim::build_array_animation({2, 1}, log);

    const auto* c = dalnim::compare_at(anim, 1.5 * kSecondsPerEvent);
    REQUIRE(c != nullptr);
    CHECK(c->box_a == 1);
    CHECK(c->box_b == 0);
}

TEST_CASE("swaps are not recorded as compares") {
    dalnim::EventLog log{dalnim::Swap{.a=0, .b=1}};
    auto anim = dalnim::build_array_animation({2, 1}, log);
    CHECK(anim.compares.empty());
}
