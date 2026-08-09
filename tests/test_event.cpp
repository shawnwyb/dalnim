#include <doctest/doctest.h>
#include "core/event.hpp"

TEST_CASE("a compare event holds two indices") {
    dalnim::Compare c{.a=1,.b=2};
    CHECK(c.a == 1);
    CHECK(c.b == 2);
}

TEST_CASE("a swap event holds two indices") {
    dalnim::Swap s{.a=1,.b=2};
    CHECK(s.a == 1);
    CHECK(s.b == 2);
}

TEST_CASE("an event log holds compares and swaps in order") {
    dalnim::EventLog log;
    log.push_back(dalnim::Compare{.a=0, .b=1});
    log.push_back(dalnim::Swap{.a=0, .b=1});

    CHECK(log.size() == 2);
    CHECK(std::holds_alternative<dalnim::Compare>(log[0]));
    CHECK(std::holds_alternative<dalnim::Swap>(log[1]));
}

TEST_CASE("an event's fields survive the round trip into the log") {
    dalnim::EventLog log;
    log.push_back(dalnim::Swap{.a=3, .b=7});

    const dalnim::Swap& s = std::get<dalnim::Swap>(log[0]);
    CHECK(s.a == 3);
    CHECK(s.b == 7);
}
TEST_CASE("a highlight event names one index") {
    dalnim::Highlight h{.index=4};
    CHECK(h.index == 4);
}

TEST_CASE("a set event names an index and its new value") {
    dalnim::Set s{.index=4, .value=-9};
    CHECK(s.index == 4);
    CHECK(s.value == -9);
}

TEST_CASE("an event log holds all four kinds") {
    dalnim::EventLog log;
    log.push_back(dalnim::Compare{.a=0, .b=1});
    log.push_back(dalnim::Swap{.a=0, .b=1});
    log.push_back(dalnim::Highlight{.index=2});
    log.push_back(dalnim::Set{.index=2, .value=7});

    CHECK(log.size() == 4);
    CHECK(std::holds_alternative<dalnim::Highlight>(log[2]));
    CHECK(std::get<dalnim::Set>(log[3]).value == 7);
}

TEST_CASE("every event kind describes itself") {
    CHECK(dalnim::describe(dalnim::Compare{.a=3, .b=5}) == "comparing 3 and 5");
    CHECK(dalnim::describe(dalnim::Swap{.a=0, .b=1}) == "swapping 0 and 1");
    CHECK(dalnim::describe(dalnim::Highlight{.index=12}) == "visiting 12");
    CHECK(dalnim::describe(dalnim::Set{.index=12, .value=1}) == "setting 12 to 1");
}

TEST_CASE("a description is never empty") {
    const dalnim::EventLog log{
        dalnim::Compare{.a=0, .b=1},
        dalnim::Swap{.a=0, .b=1},
        dalnim::Highlight{.index=0},
        dalnim::Set{.index=0, .value=-4},
    };
    for (const dalnim::Event& e : log) {
        CHECK(dalnim::describe(e).empty() == false);
    }
}
