#include "core/replay.hpp"
#include <algorithm>

namespace dalnim {
namespace {
    // How many events have finished by time t, capped at the length of the log.
    std::size_t landed(const EventLog& log, double t) {
        if (t <= 0.0) {
            return 0;
        }
        const auto count = static_cast<std::size_t>(t);
        return count > log.size() ? log.size() : count;
    }
}

std::vector<int> values_at(std::vector<int> initial, const EventLog& log, double t) {
    const std::size_t stop = landed(log, t);
    for (std::size_t k = 0; k < stop; ++k) {
        if (const auto* s = std::get_if<Set>(&log[k])) {
            if (s->index < initial.size()) {
                initial[s->index] = s->value;
            }
        }
    }
    return initial;
}

std::vector<std::optional<MarkKind>> marks_at(const EventLog& log, double t, std::size_t slots) {
    std::vector<std::optional<MarkKind>> marks(slots);
    const std::size_t stop = landed(log, t);
    for (std::size_t k = 0; k < stop; ++k) {
        if (const auto* m = std::get_if<Mark>(&log[k])) {
            if (m->index < marks.size()) {
                marks[m->index] = m->kind;
            }
        } else if (const auto* u = std::get_if<Unmark>(&log[k])) {
            if (u->index < marks.size() && marks[u->index] == u->kind) {
                marks[u->index].reset();
            }
        }
    }
    return marks;
}

std::optional<std::size_t> highlight_at(const EventLog& log, double t) {
    if (t < 0.0) {
        return std::nullopt;
    }
    const auto k = static_cast<std::size_t>(t);
    if (k >= log.size()) {
        return std::nullopt;
    }
    if (const auto* h = std::get_if<Highlight>(&log[k])) {
        return h->index;
    }
    return std::nullopt;
}

std::optional<std::size_t> sticky_highlight_at(const EventLog& log, double t) {
    std::optional<std::size_t> found;
    const std::size_t stop = landed(log, t + 1.0);
    for (std::size_t k = 0; k < stop; ++k) {
        if (const auto* h = std::get_if<Highlight>(&log[k])) {
            found = h->index;
        }
    }
    return found;
}

std::optional<std::pair<std::size_t, std::size_t>> compared_at(const EventLog& log, double t) {
    if (t < 0.0) {
        return std::nullopt;
    }
    const auto k = static_cast<std::size_t>(t);
    if (k >= log.size()) {
        return std::nullopt;
    }
    if (const auto* c = std::get_if<Compare>(&log[k])) {
        return std::pair{c->a, c->b};
    }
    return std::nullopt;
}

std::vector<std::size_t> marked_in_order_at(const EventLog& log, double t, MarkKind kind) {
    std::vector<std::size_t> waiting;
    const std::size_t stop = landed(log, t);

    for (std::size_t k = 0; k < stop; ++k) {
        if (const auto* m = std::get_if<Mark>(&log[k])) {
            if (m->kind != kind) {
                continue;
            }
            // A cell already in the line does not join it a second time.
            if (std::find(waiting.begin(), waiting.end(), m->index) == waiting.end()) {
                waiting.push_back(m->index);
            }
        } else if (const auto* u = std::get_if<Unmark>(&log[k])) {
            if (u->kind != kind) {
                continue;
            }
            const auto at = std::find(waiting.begin(), waiting.end(), u->index);
            if (at != waiting.end()) {
                waiting.erase(at);
            }
        }
    }
    return waiting;
}

std::vector<int> pile_at(const EventLog& log, double t) {
    std::vector<int> pile;
    const std::size_t stop = landed(log, t);
    for (std::size_t k = 0; k < stop; ++k) {
        if (const auto* push = std::get_if<Push>(&log[k])) {
            pile.push_back(push->value);
        } else if (std::holds_alternative<Pop>(log[k]) && !pile.empty()) {
            pile.pop_back();
        }
    }
    return pile;
}
}
