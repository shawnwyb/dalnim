#include "core/algos/registry.hpp"
#include "core/algos/bubble_sort.hpp"
#include "core/algos/selection_sort.hpp"

namespace dalnim {
namespace {
    constexpr Algorithm kAll[] = {
        {"bubble sort", &bubble_sort},
        {"selection sort", &selection_sort},
    };
}

std::span<const Algorithm> algorithms() {
    return kAll;
}
}
