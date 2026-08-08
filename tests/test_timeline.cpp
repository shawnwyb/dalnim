#include <doctest/doctest.h>
#include "core/timeline.hpp"

TEST_CASE("a keyframe holds a time and a value") {
    dalnim::Keyframe k{0.5, 200.0};
    CHECK(k.time == 0.5);
    CHECK(k.value == 200.0);
}