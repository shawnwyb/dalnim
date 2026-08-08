#include "core/grid.hpp"

namespace dalnim {
    std::size_t cell_index(const Grid& grid, std::size_t row, std::size_t col) {
        return row * grid.width + col;
    }

    bool grid_is_well_formed(const Grid& grid) {
        return grid.cells.size() == grid.width * grid.height;
    }
}
