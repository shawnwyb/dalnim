#include "core/dp_table.hpp"

namespace dalnim {
    std::size_t dp_width(const DpTable& table) {
        return table.across.size() + 1;
    }

    std::size_t dp_height(const DpTable& table) {
        return table.down.size() + 1;
    }

    std::size_t dp_cells(const DpTable& table) {
        return dp_width(table) * dp_height(table);
    }

    std::size_t dp_index(const DpTable& table, std::size_t row, std::size_t col) {
        return row * dp_width(table) + col;
    }
}
