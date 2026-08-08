#include <doctest/doctest.h>
#include "core/timeline.hpp"

TEST_CASE("a keyframe holds a time and a value") {
    dalnim::Keyframe k{.time=0.5, .value=200.0};
    CHECK(k.time == 0.5);
    CHECK(k.value == 200.0);
}

TEST_CASE("two keyframes, sample halfway") {
    dalnim::Timeline t;
    t.add(0.0, 100.0);
    t.add(1.0, 300.0);
    CHECK(t.sample(0.5) == doctest::Approx(200.0));
}

TEST_CASE("sample exactly on a keyframe") {
    dalnim::Timeline t;
    t.add(0.0, 100.0);
    t.add(1.0, 300.0);
    CHECK(t.sample(0.0) == doctest::Approx(100.0));
    CHECK(t.sample(1.0) == doctest::Approx(300.0));
}

TEST_CASE("before the first keyframe") {
    dalnim::Timeline t;
    t.add(1.0, 100.0);
    t.add(2.0, 300.0);
    CHECK(t.sample(-5.0) == doctest::Approx(100.0));
}

TEST_CASE("after the last keyframe") {
    dalnim::Timeline t;
    t.add(1.0, 100.0);
    t.add(2.0, 300.0);
    CHECK(t.sample(99.0) == doctest::Approx(300.0));
}

TEST_CASE("empty timeline") {
    dalnim::Timeline t;
    CHECK(t.sample(0.0) == doctest::Approx(0.0));
}

TEST_CASE("a single keyframe holds everywhere") {
    dalnim::Timeline t;
    t.add(1.0, 42.0);
    CHECK(t.sample(0.0) == doctest::Approx(42.0));
    CHECK(t.sample(1.0) == doctest::Approx(42.0));
    CHECK(t.sample(9.0) == doctest::Approx(42.0));
}

TEST_CASE("added out of order") {
    dalnim::Timeline t;
    t.add(1.0, 300.0);
    t.add(0.0, 100.0);
    CHECK(t.sample(0.5) == doctest::Approx(200.0));
}

TEST_CASE("three keyframes interpolate within the right segment") {
    dalnim::Timeline t;
    t.add(0.0, 0.0);
    t.add(1.0, 100.0);
    t.add(2.0, 0.0);
    CHECK(t.sample(1.5) == doctest::Approx(50.0));
}

TEST_CASE("sorts by time, not by value") {
    dalnim::Timeline t;
    t.add(0.0, 300.0);   // early time, high value
    t.add(1.0, 100.0);   // late time, low value
    CHECK(t.sample(0.0) == doctest::Approx(300.0));
}