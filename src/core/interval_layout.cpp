#include "core/interval_layout.hpp"
#include <algorithm>
#include <numeric>
#include <utility>
#include "core/replay.hpp"

namespace dalnim {
namespace {
    // Which bar is sitting in each slot once every swap up to `t` has landed. A
    // swap owns the span [k, k + 1), so it only counts here after it finishes.
    std::vector<std::size_t> bar_at_slot(const EventLog& log, std::size_t count, double t) {
        std::vector<std::size_t> bars(count);
        std::iota(bars.begin(), bars.end(), std::size_t{0});

        for (std::size_t k = 0; k < log.size(); ++k) {
            if (static_cast<double>(k) + 1.0 > t) {
                break;
            }
            if (const auto* s = std::get_if<Swap>(&log[k])) {
                if (s->a < count && s->b < count) {
                    std::swap(bars[s->a], bars[s->b]);
                }
            }
        }
        return bars;
    }
}

IntervalAnimation build_interval_animation(Intervals intervals, EventLog log) {
    IntervalAnimation anim;
    const std::size_t count = intervals.items.size();

    anim.duration = static_cast<double>(log.size());
    anim.row.resize(count);
    for (std::size_t i = 0; i < count; ++i) {
        anim.row[i].add(0.0, static_cast<double>(i) * kIntervalRowPitch);
    }

    if (count > 0) {
        const auto by_start = [](const Interval& a, const Interval& b) {
            return a.start < b.start;
        };
        const auto by_end = [](const Interval& a, const Interval& b) {
            return a.end < b.end;
        };
        anim.earliest = std::min_element(intervals.items.begin(), intervals.items.end(),
                                         by_start)->start;
        const int latest = std::max_element(intervals.items.begin(), intervals.items.end(),
                                            by_end)->end;
        anim.span_x = static_cast<double>(latest - anim.earliest) * kIntervalUnitsPerStep;
        // The last row is a bar tall, not a pitch tall, so the picture ends where
        // the bottom bar ends rather than a gap below it.
        anim.span_y = static_cast<double>(count - 1) * kIntervalRowPitch + kIntervalBarHeight;
    }

    // Slot s holds this bar. A Swap names slots, not bars, exactly as on the array.
    std::vector<std::size_t> slots(count);
    std::iota(slots.begin(), slots.end(), std::size_t{0});

    for (std::size_t k = 0; k < log.size(); ++k) {
        const double t0 = static_cast<double>(k);
        const double t1 = t0 + 1.0;

        if (const auto* c = std::get_if<Compare>(&log[k])) {
            if (c->a < count && c->b < count) {
                anim.compares.push_back(ComparePair{
                    .begin = t0,
                    .end = t1,
                    .box_a = slots[c->a],
                    .box_b = slots[c->b],
                });
            }
            continue;
        }

        const auto* s = std::get_if<Swap>(&log[k]);
        if (s == nullptr || s->a >= count || s->b >= count) {
            continue;
        }

        const double row_a = static_cast<double>(s->a) * kIntervalRowPitch;
        const double row_b = static_cast<double>(s->b) * kIntervalRowPitch;

        anim.row[slots[s->a]].add(t0, row_a);
        anim.row[slots[s->a]].add(t1, row_b);
        anim.row[slots[s->b]].add(t0, row_b);
        anim.row[slots[s->b]].add(t1, row_a);

        std::swap(slots[s->a], slots[s->b]);
    }

    anim.intervals = std::move(intervals);
    anim.log = std::move(log);
    return anim;
}

double interval_x_units(const IntervalAnimation& anim, int value) {
    return static_cast<double>(value - anim.earliest) * kIntervalUnitsPerStep;
}

std::vector<std::optional<MarkKind>> interval_marks_at(const IntervalAnimation& anim, double t) {
    const std::size_t count = anim.intervals.items.size();
    const std::vector<std::optional<MarkKind>> by_slot = marks_at(anim.log, t, count);
    const std::vector<std::size_t> bars = bar_at_slot(anim.log, count, t);

    std::vector<std::optional<MarkKind>> by_bar(count);
    for (std::size_t slot = 0; slot < count; ++slot) {
        by_bar[bars[slot]] = by_slot[slot];
    }
    return by_bar;
}

const ComparePair* interval_compare_at(const IntervalAnimation& anim, double t) {
    for (const ComparePair& c : anim.compares) {
        if (t >= c.begin && t < c.end) {
            return &c;
        }
    }
    return nullptr;
}
}
