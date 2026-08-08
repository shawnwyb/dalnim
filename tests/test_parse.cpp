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
