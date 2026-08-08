#include "core/parse.hpp"
#include <charconv>
#include <cstddef>
#include <system_error>

namespace dalnim {
namespace {
    bool is_digit(char c) {
        return c >= '0' && c <= '9';
    }
}

std::vector<int> parse_int_list(std::string_view text) {
    std::vector<int> out;
    std::size_t i = 0;

    while (i < text.size()) {
        if (!is_digit(text[i]) && text[i] != '-') {
            ++i;
            continue;
        }

        const std::size_t start = i;
        if (text[i] == '-') {
            ++i;
        }

        const std::size_t first_digit = i;
        while (i < text.size() && is_digit(text[i])) {
            ++i;
        }
        if (i == first_digit) {
            continue;
        }

        int value = 0;
        const auto result = std::from_chars(text.data() + start, text.data() + i, value);
        if (result.ec == std::errc{}) {
            out.push_back(value);
        }
    }

    return out;
}
}
