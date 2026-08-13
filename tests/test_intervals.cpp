#include <doctest/doctest.h>
#include <cstddef>
#include <utility>
#include <vector>
#include "core/algos/meeting_rooms.hpp"
#include "core/interval_layout.hpp"

namespace {
    dalnim::Intervals make(std::vector<std::pair<int, int>> pairs) {
        dalnim::Intervals data;
        for (const auto& [start, end] : pairs) {
            data.items.push_back(dalnim::Interval{.start = start, .end = end});
        }
        return data;
    }

    std::size_t count_marks(const dalnim::EventLog& log, dalnim::MarkKind kind) {
        std::size_t found = 0;
        for (const dalnim::Event& e : log) {
            if (const auto* m = std::get_if<dalnim::Mark>(&e)) {
                found += m->kind == kind ? 1 : 0;
            }
        }
        return found;
    }

    // Replaying the swaps on the original input must reproduce the order the
    // algorithm worked from, or the recording is lying about what it did.
    std::vector<int> starts_after_replay(dalnim::Intervals data,
                                         const dalnim::EventLog& log) {
        for (const dalnim::Event& e : log) {
            if (const auto* s = std::get_if<dalnim::Swap>(&e)) {
                std::swap(data.items[s->a], data.items[s->b]);
            }
        }
        std::vector<int> starts;
        for (const dalnim::Interval& item : data.items) {
            starts.push_back(item.start);
        }
        return starts;
    }
}

TEST_CASE("an empty schedule records nothing") {
    CHECK(dalnim::can_attend_all(make({})).empty());
}

TEST_CASE("a single meeting cannot clash with anything") {
    const dalnim::EventLog log = dalnim::can_attend_all(make({{0, 30}}));
    CHECK(count_marks(log, dalnim::MarkKind::Answer) == 0);
    CHECK(count_marks(log, dalnim::MarkKind::Visited) == 1);
}

TEST_CASE("the recorded swaps sort the meetings by start time") {
    const dalnim::Intervals input = make({{15, 20}, {0, 5}, {30, 40}, {8, 12}});
    const dalnim::EventLog log = dalnim::can_attend_all(input);
    CHECK(starts_after_replay(input, log) == std::vector<int>{0, 8, 15, 30});
}

TEST_CASE("a schedule that fits leaves every meeting visited and none answered") {
    const dalnim::EventLog log = dalnim::can_attend_all(make({{7, 10}, {0, 5}, {13, 20}}));
    CHECK(count_marks(log, dalnim::MarkKind::Answer) == 0);
    CHECK(count_marks(log, dalnim::MarkKind::Visited) == 3);
}

TEST_CASE("meetings touching end to end do not overlap") {
    const dalnim::EventLog log = dalnim::can_attend_all(make({{0, 10}, {10, 20}}));
    CHECK(count_marks(log, dalnim::MarkKind::Answer) == 0);
}

TEST_CASE("a clash marks both meetings and stops there") {
    const dalnim::EventLog log = dalnim::can_attend_all(make({{0, 30}, {5, 10}, {15, 20}}));
    CHECK(count_marks(log, dalnim::MarkKind::Answer) == 2);

    // The two Answer marks are the last thing recorded: the run gives up on the
    // first clash rather than checking the pairs beyond it.
    REQUIRE(log.size() >= 2);
    CHECK(std::get_if<dalnim::Mark>(&log[log.size() - 1]) != nullptr);
    CHECK(std::get_if<dalnim::Mark>(&log[log.size() - 2]) != nullptr);
}

TEST_CASE("a mark lands on the bar that moved into the slot, not the slot") {
    // Sorting puts {0,30} in slot 0 and {5,10} in slot 1, so the clash marks slots
    // 0 and 1. Those are bars 1 and 0 as they were typed, and the picture has to
    // colour the bars, not the places they ended up in.
    const dalnim::Intervals input = make({{5, 10}, {0, 30}});
    const dalnim::IntervalAnimation anim =
        dalnim::build_interval_animation(input, dalnim::can_attend_all(input));

    const auto marks = dalnim::interval_marks_at(anim, anim.duration);
    REQUIRE(marks.size() == 2);
    CHECK(marks[0] == dalnim::MarkKind::Answer);
    CHECK(marks[1] == dalnim::MarkKind::Answer);
}

TEST_CASE("a swap slides both bars between rows over its own event") {
    const dalnim::Intervals input = make({{5, 10}, {0, 30}});
    const dalnim::IntervalAnimation anim =
        dalnim::build_interval_animation(input, dalnim::can_attend_all(input));

    // Bar 0 was typed first and sorts second, so it ends a row lower than it began.
    CHECK(anim.row[0].sample(0.0) == doctest::Approx(0.0));
    CHECK(anim.row[0].sample(anim.duration) == doctest::Approx(dalnim::kIntervalRowPitch));
    CHECK(anim.row[1].sample(anim.duration) == doctest::Approx(0.0));
}

TEST_CASE("the clock spans the earliest start to the latest end") {
    const dalnim::Intervals input = make({{15, 20}, {-5, 3}, {0, 40}});
    const dalnim::IntervalAnimation anim =
        dalnim::build_interval_animation(input, dalnim::can_attend_all(input));

    CHECK(anim.earliest == -5);
    CHECK(dalnim::interval_x_units(anim, -5) == doctest::Approx(0.0));
    CHECK(anim.span_x == doctest::Approx(45.0 * dalnim::kIntervalUnitsPerStep));

    // Three rows, but the bottom one is a bar rather than a whole pitch.
    CHECK(anim.span_y ==
          doctest::Approx(2.0 * dalnim::kIntervalRowPitch + dalnim::kIntervalBarHeight));
}

TEST_CASE("an empty schedule lays out without spans or rows") {
    const dalnim::IntervalAnimation anim =
        dalnim::build_interval_animation(make({}), dalnim::EventLog{});
    CHECK(anim.row.empty());
    CHECK(anim.span_x == doctest::Approx(0.0));
    CHECK(anim.span_y == doctest::Approx(0.0));
    CHECK(dalnim::interval_marks_at(anim, 0.0).empty());
}
