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



TEST_CASE("CanCreateBigInteger")
{
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateAlphaNumericHexDigit")
{
    BigInteger largeInteger("a");
    REQUIRE("0a" == largeInteger.ToString());
}

TEST_CASE("CanCreateTwoDigitNumber")
{
    BigInteger largeInteger("1a");
    REQUIRE("1a" == largeInteger.ToString());
}

TEST_CASE("CanCreateTenFifteenDigitNumber")
{
    BigInteger largeInteger("123456789abcdef");
    REQUIRE("0123456789abcdef" == largeInteger.ToString());
}

TEST_CASE("CanCreate132BitNumber")
{
    string veryLargeNumber = "0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8";
    BigInteger veryLargeInteger(veryLargeNumber);
    REQUIRE(veryLargeNumber == veryLargeInteger.ToString());
}

TEST_CASE("FailsToCreateIfCharactersInvalid")
{
    REQUIRE_THROWS(BigInteger("X"));
}

