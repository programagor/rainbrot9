#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../quaternion.hpp"

TEST_CASE("Quaternion set and get") {
    Quaternion q;
    q.set(1.0, 2.0, 3.0, 4.0);
    double r, i, j, k;
    q.get(&r, &i, &j, &k);
    REQUIRE(r == Approx(1.0));
    REQUIRE(i == Approx(2.0));
    REQUIRE(j == Approx(3.0));
    REQUIRE(k == Approx(4.0));
}
