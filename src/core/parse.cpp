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

Graph parse_graph(std::string_view text) {
    if (text.find_first_not_of(" \t\r\n") == std::string_view::npos) {
        return Graph{};
    }

    std::vector<std::vector<int>> lines;
    std::size_t start = 0;

    while (start <= text.size()) {
        const std::size_t newline = text.find('\n', start);
        const std::size_t stop = newline == std::string_view::npos ? text.size() : newline;
        lines.push_back(parse_int_list(text.substr(start, stop - start)));
        if (newline == std::string_view::npos) {
            break;
        }
        start = newline + 1;
    }

    // A final newline ends the last line rather than starting an empty one. Empty
    // lines before that are real: they are nodes with no edges of their own.
    if (!text.empty() && text.back() == '\n' && !lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }

    // A node mentioned only as somebody else's neighbour still exists.
    std::size_t count = lines.size();
    for (const std::vector<int>& line : lines) {
        for (int raw : line) {
            if (raw >= 0 && static_cast<std::size_t>(raw) + 1 > count) {
                count = static_cast<std::size_t>(raw) + 1;
            }
        }
    }

    Graph graph;
    graph.neighbours.resize(count);
    for (std::size_t from = 0; from < lines.size(); ++from) {
        for (int raw : lines[from]) {
            if (raw < 0) {
                continue;
            }
            const auto to = static_cast<std::size_t>(raw);
            if (to >= count || to == from) {
                continue;
            }
            if (!has_edge(graph, from, to)) {
                graph.neighbours[from].push_back(to);
            }
            if (!has_edge(graph, to, from)) {
                graph.neighbours[to].push_back(from);
            }
        }
    }
    return graph;
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
