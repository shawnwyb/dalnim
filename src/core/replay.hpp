#pragma once
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "core/event.hpp"

namespace dalnim {
    // Reading a log at a moment. Every view whose things do not move works this way:
    // start from the beginning and apply whatever has already happened.
    //
    // Event k owns the span [k, k + 1). A write lands at the end of its own span; a
    // highlight or comparison shows only during it.

    std::vector<int> values_at(std::vector<int> initial, const EventLog& log, double t);

    std::vector<std::optional<MarkKind>> marks_at(const EventLog& log, double t,
                                                  std::size_t slots);

    // What is being looked at during this event only.
    std::optional<std::size_t> highlight_at(const EventLog& log, double t);

    // The most recent thing looked at, which stays put until the next one.
    std::optional<std::size_t> sticky_highlight_at(const EventLog& log, double t);

    std::optional<std::pair<std::size_t, std::size_t>> compared_at(const EventLog& log, double t);

    std::vector<int> pile_at(const EventLog& log, double t);
}
