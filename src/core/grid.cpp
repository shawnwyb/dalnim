#include "core/grid.hpp"

namespace dalnim {
    std::size_t cell_index(const Grid& grid, std::size_t row, std::size_t col) {
        return row * grid.width + col;
    }

    bool grid_is_well_formed(const Grid& grid) {
        return grid.cells.size() == grid.width * grid.height;
    }

    std::vector<std::size_t> neighbours(const Grid& grid, std::size_t index) {
        std::vector<std::size_t> found;
        if (grid.width == 0 || index >= grid.cells.size()) {
            return found;
        }

        const std::size_t row = index / grid.width;
        const std::size_t col = index % grid.width;
        if (col + 1 < grid.width) {
            found.push_back(index + 1);
        }
        if (col > 0) {
            found.push_back(index - 1);
        }
        if (row + 1 < grid.height) {
            found.push_back(index + grid.width);
        }
        if (row > 0) {
            found.push_back(index - grid.width);
        }
        return found;
    }
}
