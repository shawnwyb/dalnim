#pragma once
#include <string_view>
#include <vector>

namespace dalnim {
    // Pulls every integer out of `text`, ignoring whatever separates them.
    // Anything that is not a well-formed int is skipped rather than reported.
    std::vector<int> parse_int_list(std::string_view text);
}
