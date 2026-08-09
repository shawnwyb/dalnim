#pragma once
#include <cstddef>
#include <string>

namespace dalnim {
    // Two words down the edges of a table. The table has one extra row and column
    // for the empty prefix, which is where the base cases live.
    struct DpTable {
        std::string across;
        std::string down;
    };

    std::size_t dp_width(const DpTable& table);
    std::size_t dp_height(const DpTable& table);
    std::size_t dp_cells(const DpTable& table);
    std::size_t dp_index(const DpTable& table, std::size_t row, std::size_t col);
}
