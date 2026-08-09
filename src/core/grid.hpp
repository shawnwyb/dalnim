#pragma once
#include <cstddef>
#include <vector>

namespace dalnim {
    // Cells are stored row by row, so an event naming index i means row i / width.
    struct Grid {
        std::size_t width = 0;
        std::size_t height = 0;
        std::vector<int> cells;
    };

    std::size_t cell_index(const Grid& grid, std::size_t row, std::size_t col);
    bool grid_is_well_formed(const Grid& grid);

    // The up-to-four cells reachable in one step, in a fixed order so that any two
    // algorithms walking the same grid produce logs you can compare.
    std::vector<std::size_t> neighbours(const Grid& grid, std::size_t index);
}
