#include "core/algos/has_duplicate.hpp"
#include <cstddef>
#include <unordered_map>

namespace dalnim {
    EventLog has_duplicate(std::vector<int> data) {
        EventLog log;

        // A set is all the algorithm needs, but remembering which value came from
        // which slot lets the picture point back at the earlier one when the answer
        // turns up. The extra field changes nothing about what is decided or when.
        std::unordered_map<int, std::size_t> seen;

        for (std::size_t i = 0; i < data.size(); ++i) {
            log.push_back(Highlight{.index = i});

            const auto found = seen.find(data[i]);
            if (found != seen.end()) {
                log.push_back(Compare{.a = found->second, .b = i});
                log.push_back(Mark{.index = found->second, .kind = MarkKind::Answer});
                log.push_back(Mark{.index = i, .kind = MarkKind::Answer});
                return log;
            }

            seen.emplace(data[i], i);
            log.push_back(Push{.value = data[i]});
        }

        return log;
    }
}
