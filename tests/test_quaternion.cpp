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

TEST_CASE("Quaternion arithmetic") {
    Quaternion a; a.set(1.0, 2.0, 3.0, 4.0);
    Quaternion b; b.set(5.0, 6.0, 7.0, 8.0);
    Quaternion sum;
    quaternion_add(a,b,sum);
    double r,i,j,k;
    sum.get(&r,&i,&j,&k);
    REQUIRE(r == Approx(6.0));
    REQUIRE(i == Approx(8.0));
    REQUIRE(j == Approx(10.0));
    REQUIRE(k == Approx(12.0));

    Quaternion prod;
    quaternion_mul(a,b,prod);
    prod.get(&r,&i,&j,&k);
    REQUIRE(r == Approx(-60.0));
    REQUIRE(i == Approx(12.0));
    REQUIRE(j == Approx(30.0));
    REQUIRE(k == Approx(24.0));
}
