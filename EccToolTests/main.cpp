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



TEST_CASE("CanCreateBigIntegerWithHexString")
{
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithInteger")
{
    unsigned int number = 1;
    BigInteger one(number);
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

TEST_CASE("CanAddTwoBigIntegers")
{
    BigInteger left("1");
    BigInteger right("1");
    
    BigInteger sum = left + right;
    REQUIRE("02" == sum.ToString());
}

TEST_CASE("CanAddTwoLargerBigIntegers")
{
    BigInteger left("12345");
    BigInteger right("67890");
    
    BigInteger sum = left + right;
    REQUIRE("079bd5" == sum.ToString());
}

TEST_CASE("CanAddIntegralTypeToBigInteger")
{
    BigInteger left("500");
    unsigned int right = 5;
    
    BigInteger sum = left + right;
    REQUIRE("0505" == sum.ToString());
}

TEST_CASE("FailsToCreateIfCharactersInvalid")
{
    REQUIRE_THROWS(BigInteger("X"));
}

TEST_CASE("CanPrefixIncrement")
{
    BigInteger testInteger("5");
    
    REQUIRE("06" == (++testInteger).ToString());
    REQUIRE("06" == testInteger.ToString());
}

TEST_CASE("CanPostfixIncrement")
{
    BigInteger testInteger("5");
    
    REQUIRE("05" == (testInteger++).ToString());
    REQUIRE("06" == testInteger.ToString());
}

TEST_CASE("ShouldRemoveLeadingZeros")
{
    BigInteger testInteger("005");
    
    REQUIRE("05" == testInteger.ToString());
}

TEST_CASE("CanDetermineShorterIntegerAsSmaller")
{
    BigInteger smaller("5201");
    BigInteger larger("72011");
    
    REQUIRE(smaller < larger);
}

TEST_CASE("CanDetermineLongerIntegerAsLarger")
{
    BigInteger smaller("5201");
    BigInteger larger("72011");
    
    REQUIRE_FALSE(larger < smaller);
}

TEST_CASE("CanDetermineLargerSameLengthIntegerAsLarger")
{
    BigInteger smaller("5201");
    BigInteger larger("5202");
    
    REQUIRE(smaller < larger);
}

TEST_CASE("CanSubtractSmallNumbers")
{
    BigInteger top("5");
    BigInteger bottom("4");
    
    BigInteger difference = top - bottom;
    REQUIRE("01" == difference.ToString());
}

TEST_CASE("CanSubtractEqualNumbers")
{
    BigInteger top("5");
    BigInteger bottom("5");
    
    BigInteger difference = top - bottom;
    REQUIRE("00" == difference.ToString());
}

TEST_CASE("CanSubtractLargeNumbers")
{
    BigInteger top("1234678905");
    BigInteger bottom("1234678904");
    
    BigInteger difference = top - bottom;
    REQUIRE("01" == difference.ToString());
}









