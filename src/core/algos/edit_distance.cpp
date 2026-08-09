#include "core/algos/edit_distance.hpp"
#include <vector>

namespace dalnim {
    EventLog edit_distance(DpTable table) {
        EventLog log;
        const std::size_t width = dp_width(table);
        const std::size_t height = dp_height(table);
        if (table.across.empty() && table.down.empty()) {
            return log;
        }

        std::vector<int> best(dp_cells(table), 0);

        // Turning a word into nothing costs one deletion per character, and the
        // other way round costs one insertion, so the first row and column count up.
        for (std::size_t col = 0; col < width; ++col) {
            const std::size_t cell = col;
            log.push_back(Highlight{.index = cell});
            best[cell] = static_cast<int>(col);
            log.push_back(Set{.index = cell, .value = best[cell]});
        }
        for (std::size_t row = 1; row < height; ++row) {
            const std::size_t cell = row * width;
            log.push_back(Highlight{.index = cell});
            best[cell] = static_cast<int>(row);
            log.push_back(Set{.index = cell, .value = best[cell]});
        }

        for (std::size_t row = 1; row < height; ++row) {
            for (std::size_t col = 1; col < width; ++col) {
                const std::size_t cell = row * width + col;
                const std::size_t diagonal = cell - width - 1;
                const std::size_t above = cell - width;
                const std::size_t left = cell - 1;

                log.push_back(Highlight{.index = cell});

                std::size_t from = diagonal;
                int value = 0;
                if (table.down[row - 1] == table.across[col - 1]) {
                    value = best[diagonal];
                } else {
                    value = best[diagonal];
                    if (best[above] < value) {
                        value = best[above];
                        from = above;
                    }
                    if (best[left] < value) {
                        value = best[left];
                        from = left;
                    }
                    value += 1;
                }

                log.push_back(Compare{.a = from, .b = cell});
                best[cell] = value;
                log.push_back(Set{.index = cell, .value = value});
            }
        }

        return log;
    }
}
