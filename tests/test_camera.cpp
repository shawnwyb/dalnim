#include <doctest/doctest.h>
#include "render/camera.hpp"

using dalnim::Camera;
using dalnim::fit_row;

TEST_CASE("content that already fits is not enlarged") {
    const Camera c = fit_row(200.0, 900.0f, 40.0f, 0.1f);
    CHECK(c.scale == doctest::Approx(1.0f));
}

TEST_CASE("content that fits is centred") {
    const Camera c = fit_row(200.0, 900.0f, 40.0f, 0.1f);
    CHECK(c.origin_x == doctest::Approx(350.0f));
    CHECK(c.x(200.0) == doctest::Approx(550.0f));
}

TEST_CASE("oversized content shrinks to the usable width") {
    const Camera c = fit_row(1640.0, 900.0f, 40.0f, 0.01f);
    CHECK(c.scale == doctest::Approx(0.5f));
    CHECK(c.origin_x == doctest::Approx(40.0f));
    CHECK(c.x(1640.0) == doctest::Approx(860.0f));
}

TEST_CASE("the scale never falls below the floor") {
    const Camera c = fit_row(100000.0, 900.0f, 40.0f, 0.1f);
    CHECK(c.scale == doctest::Approx(0.1f));
}

TEST_CASE("empty content is centred and unscaled") {
    const Camera c = fit_row(0.0, 900.0f, 40.0f, 0.1f);
    CHECK(c.scale == doctest::Approx(1.0f));
    CHECK(c.origin_x == doctest::Approx(450.0f));
}

TEST_CASE("the origin is where unit zero lands") {
    const Camera c = fit_row(1640.0, 900.0f, 40.0f, 0.01f);
    CHECK(c.x(0.0) == doctest::Approx(c.origin_x));
}

TEST_CASE("lengths scale but carry no origin") {
    const Camera c = fit_row(1640.0, 900.0f, 40.0f, 0.01f);
    CHECK(c.length(100.0) == doctest::Approx(50.0f));
    CHECK(c.length(0.0) == doctest::Approx(0.0f));
}
