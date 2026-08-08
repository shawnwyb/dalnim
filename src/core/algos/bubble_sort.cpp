#include "core/algos/bubble_sort.hpp"
#include <cstddef>
#include <utility>

namespace dalnim {
    EventLog bubble_sort(std::vector<int> data) {
        EventLog log;
        for (std::size_t i = 0; i + 1 < data.size(); ++i) {
            for (std::size_t j = 0; j + 1 < data.size() - i; ++j) {
                log.push_back(Compare{.a=j, .b=j + 1});
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                    log.push_back(Swap{.a=j, .b=j + 1});
                }
            }
        }
        return log;
    }
}
