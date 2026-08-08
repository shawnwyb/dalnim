#include <doctest/doctest.h>
#include "core/ease.hpp"

using dalnim::smooth;

TEST_CASE("the endpoints are untouched") {
    CHECK(smooth(0.0) == doctest::Approx(0.0));
    CHECK(smooth(1.0) == doctest::Approx(1.0));
}

TEST_CASE("the midpoint is untouched") {
    CHECK(smooth(0.5) == doctest::Approx(0.5));
}

TEST_CASE("it starts slow and ends slow") {
    CHECK(smooth(0.25) < 0.25);
    CHECK(smooth(0.75) > 0.75);
}

TEST_CASE("it never goes backwards") {
    double previous = -1.0;
    for (int i = 0; i <= 100; ++i) {
        const double current = smooth(i / 100.0);
        CHECK(current >= previous);
        previous = current;
    }
}

TEST_CASE("it stays inside 0..1 even when fed nonsense") {
    CHECK(smooth(-3.0) == doctest::Approx(0.0));
    CHECK(smooth(7.0) == doctest::Approx(1.0));
}
