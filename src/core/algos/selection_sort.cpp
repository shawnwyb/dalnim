#include "core/algos/selection_sort.hpp"
#include <cstddef>
#include <utility>

namespace dalnim {
    EventLog selection_sort(std::vector<int> data) {
        EventLog log;
        for (std::size_t i = 0; i + 1 < data.size(); ++i) {
            std::size_t smallest = i;
            for (std::size_t j = i + 1; j < data.size(); ++j) {
                log.push_back(Compare{.a = smallest, .b = j});
                if (data[j] < data[smallest]) {
                    smallest = j;
                }
            }
            if (smallest != i) {
                std::swap(data[i], data[smallest]);
                log.push_back(Swap{.a = i, .b = smallest});
            }
        }
        return log;
    }
}
