#include "core/parse.hpp"
#include <charconv>
#include <cstddef>
#include <system_error>
#include <utility>
#include <vector>

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

Tree parse_tree(std::string_view text) {
    Tree tree;
    std::size_t i = 0;

    while (i < text.size()) {
        while (i < text.size() && (text[i] == ' ' || text[i] == ',' ||
                                   text[i] == '\n' || text[i] == '\t' || text[i] == '\r')) {
            ++i;
        }
        if (i >= text.size()) {
            break;
        }

        const std::size_t start = i;
        while (i < text.size() && text[i] != ' ' && text[i] != ',' &&
               text[i] != '\n' && text[i] != '\t' && text[i] != '\r') {
            ++i;
        }

        const std::vector<int> parsed = parse_int_list(text.substr(start, i - start));
        tree.values.push_back(parsed.empty() ? 0 : parsed.front());
        tree.present.push_back(!parsed.empty());
    }

    // Trailing empties describe nothing, so drop them.
    while (!tree.present.empty() && !tree.present.back()) {
        tree.present.pop_back();
        tree.values.pop_back();
    }
    return tree;
}

Grid parse_grid(std::string_view text) {
    std::vector<std::vector<int>> rows;
    std::size_t start = 0;

    while (start <= text.size()) {
        const std::size_t newline = text.find('\n', start);
        const std::size_t stop = newline == std::string_view::npos ? text.size() : newline;

        std::vector<int> row = parse_int_list(text.substr(start, stop - start));
        if (!row.empty()) {
            rows.push_back(std::move(row));
        }

        if (newline == std::string_view::npos) {
            break;
        }
        start = newline + 1;
    }

    Grid grid;
    for (const std::vector<int>& row : rows) {
        grid.width = row.size() > grid.width ? row.size() : grid.width;
    }
    grid.height = rows.size();
    grid.cells.assign(grid.width * grid.height, 0);

    for (std::size_t r = 0; r < rows.size(); ++r) {
        for (std::size_t c = 0; c < rows[r].size(); ++c) {
            grid.cells[r * grid.width + c] = rows[r][c];
        }
    }
    return grid;
}
}
