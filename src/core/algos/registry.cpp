#include "core/algos/registry.hpp"
#include <utility>
#include "core/algos/bubble_sort.hpp"
#include "core/algos/flood_fill.hpp"
#include "core/algos/selection_sort.hpp"

namespace dalnim {
namespace {
    EventLog flood_fill_with_one(Grid grid, std::size_t start) {
        return flood_fill(std::move(grid), start, 1);
    }

    constexpr Algorithm kAll[] = {
        {"bubble sort", ArrayAlgorithm{&bubble_sort}},
        {"selection sort", ArrayAlgorithm{&selection_sort}},
        {"flood fill", GridAlgorithm{&flood_fill_with_one}},
    };
}

bool wants_array(const Algorithm& algo) {
    return std::holds_alternative<ArrayAlgorithm>(algo.run);
}

std::span<const Algorithm> algorithms() {
    return kAll;
}
}
