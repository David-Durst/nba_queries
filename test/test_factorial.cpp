#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "factorial.h"

TEST_CASE( "verifying factorial", "[factorial]" ) {
    SECTION( "testing factorial 0" ) {
        REQUIRE( factorial(0) == 1);
    }
    SECTION( "testing factorial 1" ) {
        REQUIRE( factorial(1) == 1);
    }
    SECTION( "testing factorial 2" ) {
        REQUIRE( factorial(2) == 2);
    }
    SECTION( "testing factorial 3" ) {
        REQUIRE( factorial(3) == 6);
    }
}
