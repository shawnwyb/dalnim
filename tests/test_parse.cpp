#include <doctest/doctest.h>
#include <vector>
#include "core/parse.hpp"

using dalnim::parse_int_list;

TEST_CASE("empty text yields nothing") {
    CHECK(parse_int_list("").empty());
}

TEST_CASE("commas separate values") {
    CHECK(parse_int_list("5,3,8") == std::vector<int>{5, 3, 8});
}

TEST_CASE("spaces separate values just as well") {
    CHECK(parse_int_list("5 3 8") == std::vector<int>{5, 3, 8});
}

TEST_CASE("surrounding and repeated separators are ignored") {
    CHECK(parse_int_list("  5, , 3 ,,8  ") == std::vector<int>{5, 3, 8});
}

TEST_CASE("negatives are kept") {
    CHECK(parse_int_list("-2, 7, -11") == std::vector<int>{-2, 7, -11});
}

TEST_CASE("text with no digits yields nothing") {
    CHECK(parse_int_list("hello there").empty());
}

TEST_CASE("a lone minus is not a number") {
    CHECK(parse_int_list("-, 4") == std::vector<int>{4});
}

TEST_CASE("a value too large for an int is skipped, not truncated") {
    CHECK(parse_int_list("99999999999999999999, 4") == std::vector<int>{4});
}

TEST_CASE("a single value parses") {
    CHECK(parse_int_list("42") == std::vector<int>{42});
}

TEST_CASE("an empty grid parses to nothing") {
    const dalnim::Grid g = dalnim::parse_grid("");
    CHECK(g.width == 0);
    CHECK(g.height == 0);
    CHECK(g.cells.empty());
}

TEST_CASE("one line per row") {
    const dalnim::Grid g = dalnim::parse_grid("1 2 3\n4 5 6");
    CHECK(g.width == 3);
    CHECK(g.height == 2);
    CHECK(g.cells == std::vector<int>{1, 2, 3, 4, 5, 6});
}

TEST_CASE("commas work in a grid too") {
    const dalnim::Grid g = dalnim::parse_grid("1,2\n3,4");
    CHECK(g.cells == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("blank lines are skipped") {
    const dalnim::Grid g = dalnim::parse_grid("\n1 2\n\n3 4\n\n");
    CHECK(g.height == 2);
    CHECK(g.cells == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("short rows are padded with zeroes") {
    const dalnim::Grid g = dalnim::parse_grid("1 2 3\n4");
    CHECK(g.width == 3);
    CHECK(g.height == 2);
    CHECK(g.cells == std::vector<int>{1, 2, 3, 4, 0, 0});
}

TEST_CASE("a parsed grid is always rectangular") {
    const dalnim::Grid g = dalnim::parse_grid("1\n2 3 4 5\n6 7");
    CHECK(dalnim::grid_is_well_formed(g));
    CHECK(g.width == 4);
    CHECK(g.height == 3);
}

TEST_CASE("one interval per line, start then end") {
    const dalnim::Intervals i = dalnim::parse_intervals("0 30\n5 10\n15 20");
    REQUIRE(i.items.size() == 3);
    CHECK(i.items[0].start == 0);
    CHECK(i.items[0].end == 30);
    CHECK(i.items[2].start == 15);
    CHECK(i.items[2].end == 20);
}

TEST_CASE("a line without two numbers is not an interval") {
    const dalnim::Intervals i = dalnim::parse_intervals("0 30\n\n7\nnonsense\n1 2");
    REQUIRE(i.items.size() == 2);
    CHECK(i.items[1].start == 1);
}

TEST_CASE("a backwards interval is turned around") {
    const dalnim::Intervals i = dalnim::parse_intervals("30 0");
    REQUIRE(i.items.size() == 1);
    CHECK(i.items[0].start == 0);
    CHECK(i.items[0].end == 30);
}

TEST_CASE("extra numbers on a line are ignored") {
    const dalnim::Intervals i = dalnim::parse_intervals("1 2 3 4");
    REQUIRE(i.items.size() == 1);
    CHECK(i.items[0].end == 2);
}

TEST_CASE("negative starts are kept") {
    const dalnim::Intervals i = dalnim::parse_intervals("-5 -1");
    REQUIRE(i.items.size() == 1);
    CHECK(i.items[0].start == -5);
    CHECK(i.items[0].end == -1);
}
