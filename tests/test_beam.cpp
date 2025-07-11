#include "catch.hpp"
#include "../beam.hpp"

TEST_CASE("Beam state transitions") {
    Beam b;
    REQUIRE(b.state == Beam::Idle);
    b.start();
    REQUIRE(b.state == Beam::Running);
    b.pause();
    REQUIRE(b.state == Beam::Paused);
    b.reset();
    REQUIRE(b.state == Beam::Idle);
}
