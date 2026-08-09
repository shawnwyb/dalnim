#pragma once
#include <string_view>
#include <vector>
#include "core/grid.hpp"

namespace dalnim {
    // Pulls every integer out of `text`, ignoring whatever separates them.
    // Anything that is not a well-formed int is skipped rather than reported.
    std::vector<int> parse_int_list(std::string_view text);

    // One row per line, blank lines skipped. Short rows are padded with zeroes so
    // the result is always rectangular.
    Grid parse_grid(std::string_view text);
}
