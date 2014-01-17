//
//  main.cpp
//  EccToolTests
//
//  Created by Josh Strom on 1/12/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "EccAlg.h"
#include "BigInteger.h"



TEST_CASE("ShouldCreateBigInteger") {
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("FailsToCreateIfCharactersInvalid")
{
    REQUIRE_THROWS(BigInteger("X"));
}

