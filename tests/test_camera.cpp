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

TEST_CASE("a box that already fits is not enlarged") {
    const Camera c = dalnim::fit_box(200.0, 100.0, 900.0f, 600.0f, 40.0f, 40.0f, 0.1f);
    CHECK(c.scale == doctest::Approx(1.0f));
}

TEST_CASE("a box is limited by whichever axis is tighter") {
    // width would allow 2.0, height only allows 0.5
    const Camera c = dalnim::fit_box(410.0, 1040.0, 900.0f, 600.0f, 40.0f, 40.0f, 0.01f);
    CHECK(c.scale == doctest::Approx(0.5f));
}

TEST_CASE("a box is centred on both axes") {
    const Camera c = dalnim::fit_box(200.0, 100.0, 900.0f, 600.0f, 40.0f, 40.0f, 0.1f);
    CHECK(c.origin_x == doctest::Approx(350.0f));
    CHECK(c.origin_y == doctest::Approx(250.0f));
    CHECK(c.x(0.0) == doctest::Approx(c.origin_x));
    CHECK(c.y(0.0) == doctest::Approx(c.origin_y));
}

TEST_CASE("both axes share one scale so squares stay square") {
    const Camera c = dalnim::fit_box(2000.0, 2000.0, 900.0f, 600.0f, 0.0f, 0.0f, 0.01f);
    CHECK(c.length(100.0) == doctest::Approx(c.length(100.0)));
    CHECK(c.x(100.0) - c.x(0.0) == doctest::Approx(c.y(100.0) - c.y(0.0)));
}

TEST_CASE("an empty box is centred and unscaled") {
    const Camera c = dalnim::fit_box(0.0, 0.0, 900.0f, 600.0f, 40.0f, 40.0f, 0.1f);
    CHECK(c.scale == doctest::Approx(1.0f));
    CHECK(c.origin_x == doctest::Approx(450.0f));
    CHECK(c.origin_y == doctest::Approx(300.0f));
}

TEST_CASE("fit_row leaves the vertical origin alone") {
    const Camera c = fit_row(200.0, 900.0f, 40.0f, 0.1f);
    CHECK(c.origin_y == doctest::Approx(0.0f));
}
