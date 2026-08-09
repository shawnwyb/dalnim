#include "core/algos/monotonic_stack.hpp"
#include <cstddef>

namespace dalnim {
    EventLog next_greater(std::vector<int> data) {
        EventLog log;
        std::vector<std::size_t> stack;

        for (std::size_t i = 0; i < data.size(); ++i) {
            log.push_back(Highlight{.index = i});

            while (!stack.empty()) {
                log.push_back(Compare{.a = stack.back(), .b = i});
                if (data[stack.back()] >= data[i]) {
                    break;
                }
                stack.pop_back();
                log.push_back(Pop{});
            }

            stack.push_back(i);
            log.push_back(Push{.value = data[i]});
        }

        return log;
    }
}
