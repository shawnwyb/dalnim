#include <doctest/doctest.h>
#include <string>
#include <vector>
#include "core/algos/edit_distance.hpp"
#include "core/dp_layout.hpp"
#include "core/parse.hpp"

namespace {
    dalnim::DpTable words(std::string across, std::string down) {
        return dalnim::DpTable{.across = std::move(across), .down = std::move(down)};
    }

    // The bottom right cell holds the answer once the run has finished.
    int answer(const dalnim::DpTable& table) {
        const auto anim = dalnim::build_dp_animation(table, dalnim::edit_distance(table));
        const std::vector<int> values = dalnim::dp_values_at(anim, anim.duration);
        return values.back();
    }
}

TEST_CASE("the table has a spare row and column for the empty prefix") {
    const dalnim::DpTable table = words("ab", "xyz");
    CHECK(dalnim::dp_width(table) == 3);
    CHECK(dalnim::dp_height(table) == 4);
    CHECK(dalnim::dp_cells(table) == 12);
    CHECK(dalnim::dp_index(table, 2, 1) == 7);
}

TEST_CASE("two words are read one per line") {
    const dalnim::DpTable table = dalnim::parse_dp_table("kitten\nsitting");
    CHECK(table.across == "kitten");
    CHECK(table.down == "sitting");
}

TEST_CASE("spaces around the words are ignored") {
    const dalnim::DpTable table = dalnim::parse_dp_table("  cat \n dog  ");
    CHECK(table.across == "cat");
    CHECK(table.down == "dog");
}

TEST_CASE("a missing second line leaves it empty") {
    const dalnim::DpTable table = dalnim::parse_dp_table("cat");
    CHECK(table.across == "cat");
    CHECK(table.down.empty());
}

TEST_CASE("two empty words produce no events") {
    CHECK(dalnim::edit_distance(words("", "")).empty());
}

TEST_CASE("turning a word into nothing costs one edit per letter") {
    CHECK(answer(words("cat", "")) == 3);
    CHECK(answer(words("", "dog")) == 3);
}

TEST_CASE("identical words cost nothing") {
    CHECK(answer(words("kitten", "kitten")) == 0);
}

TEST_CASE("one substitution costs one") {
    CHECK(answer(words("cat", "cut")) == 1);
}

TEST_CASE("one insertion costs one") {
    CHECK(answer(words("cat", "cart")) == 1);
}

TEST_CASE("the textbook answer comes out right") {
    CHECK(answer(words("kitten", "sitting")) == 3);
    CHECK(answer(words("sunday", "saturday")) == 3);
}

TEST_CASE("the answer does not depend on which word goes across") {
    CHECK(answer(words("flaw", "lawn")) == answer(words("lawn", "flaw")));
}

TEST_CASE("no cell is filled before the run begins") {
    const dalnim::DpTable table = words("cat", "cut");
    const auto anim = dalnim::build_dp_animation(table, dalnim::edit_distance(table));
    for (bool filled : dalnim::dp_filled_at(anim, 0.0)) {
        CHECK(filled == false);
    }
}

TEST_CASE("every cell is filled by the end") {
    const dalnim::DpTable table = words("cat", "cut");
    const auto anim = dalnim::build_dp_animation(table, dalnim::edit_distance(table));
    for (bool filled : dalnim::dp_filled_at(anim, anim.duration)) {
        CHECK(filled);
    }
}

TEST_CASE("a cell is only ever filled from cells already filled") {
    const dalnim::DpTable table = words("kitten", "sitting");
    const auto anim = dalnim::build_dp_animation(table, dalnim::edit_distance(table));

    for (double t = 0.0; t <= anim.duration; t += 1.0) {
        const auto source = dalnim::dp_source_at(anim, t);
        if (!source.has_value()) {
            continue;
        }
        const std::vector<bool> filled = dalnim::dp_filled_at(anim, t);
        CAPTURE(t);
        CHECK(filled[source->first]);
    }
}

TEST_CASE("a source cell always sits above or to the left") {
    const dalnim::DpTable table = words("kitten", "sitting");
    const std::size_t width = dp_width(table);

    for (const dalnim::Event& e : dalnim::edit_distance(table)) {
        if (const auto* c = std::get_if<dalnim::Compare>(&e)) {
            CHECK(c->a < c->b);
            const std::size_t from_row = c->a / width;
            const std::size_t to_row = c->b / width;
            const std::size_t from_col = c->a % width;
            const std::size_t to_col = c->b % width;
            CHECK(from_row <= to_row);
            CHECK(from_col <= to_col);
        }
    }
}

TEST_CASE("every event names a cell inside the table") {
    const dalnim::DpTable table = words("cat", "cut");
    const std::size_t cells = dp_cells(table);

    for (const dalnim::Event& e : dalnim::edit_distance(table)) {
        if (const auto* s = std::get_if<dalnim::Set>(&e)) {
            CHECK(s->index < cells);
        }
        if (const auto* h = std::get_if<dalnim::Highlight>(&e)) {
            CHECK(h->index < cells);
        }
        if (const auto* c = std::get_if<dalnim::Compare>(&e)) {
            CHECK(c->a < cells);
            CHECK(c->b < cells);
        }
    }
}

TEST_CASE("no value in the table is negative") {
    const dalnim::DpTable table = words("kitten", "sitting");
    const auto anim = dalnim::build_dp_animation(table, dalnim::edit_distance(table));
    for (int value : dalnim::dp_values_at(anim, anim.duration)) {
        CHECK(value >= 0);
    }
}
