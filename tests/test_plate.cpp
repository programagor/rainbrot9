#include "catch.hpp"
#include "../plate.hpp"

TEST_CASE("Plate creation and deletion") {
    Plate p(64, 64);
    REQUIRE(p.width == 64);
    REQUIRE(p.height == 64);
    // Insert a simple photon to ensure no crash
    Quaternion q; q.set(0.0,0.0,0.0,0.0);
    REQUIRE_NOTHROW(p.insertPhoton(q, 1));
}
