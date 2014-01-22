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

void AddTwoNumbersTest(unsigned int number1, unsigned int number2, int count = -1)
{
    unsigned long sum = number1 + number2;
    
    stringstream ss;
    ss << hex << sum;
    
    string sumInHex = ss.str();
    ss.clear();
    
    if((sumInHex.size() % 2) != 0)
        sumInHex.insert(sumInHex.begin(), '0');
    
    if(count != -1)
    {
        ss << ", Counter: " << count;
    }
    
    string actualSumInHex = (BigInteger(number1) + BigInteger(number2)).ToString();
    if(actualSumInHex != sumInHex)
        FAIL("Operation " << hex << number1 << " + " << number2 << " resulted in " << actualSumInHex << ". Should have been: " << sumInHex << ss.str());
}

TEST_CASE("CanCreateBigIntegerWithHexString")
{
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("ThrowsOnInvalidBase")
{
    REQUIRE_THROWS(BigInteger("5x"));
}

TEST_CASE("CanCreateBigIntegerWithChar")
{
    char number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithUnsignedChar")
{
    unsigned char number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithShort")
{
    short number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithUnsignedShort")
{
    unsigned short number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithInteger")
{
    int number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithUnsignedInteger")
{
    unsigned int number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithLong")
{
    long number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithUnsignedLong")
{
    unsigned long number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithLongLong")
{
    long long number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateBigIntegerWithUnsignedLongLong")
{
    unsigned long long number = 1;
    BigInteger one(number);
    REQUIRE("01" == one.ToString());
}

TEST_CASE("ThrowsIfCreatedFromNegativeInteger")
{
    REQUIRE_THROWS(BigInteger(-5));
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

TEST_CASE("CanCreateLargeIntegerFromInteger")
{
    REQUIRE(BigInteger(0xFFFFFFFF).ToString() == "ffffffff");
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
    int right = 5;
    
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

TEST_CASE("CanPrefixDecrement")
{
    BigInteger testInteger("5");
    
    REQUIRE("04" == (--testInteger).ToString());
    REQUIRE("04" == testInteger.ToString());
}

TEST_CASE("CanPostfixDecrement")
{
    BigInteger testInteger("5");
    
    REQUIRE("05" == (testInteger--).ToString());
    REQUIRE("04" == testInteger.ToString());
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

TEST_CASE("CanDetectEquality")
{
    REQUIRE(BigInteger(5) == BigInteger(5));
    REQUIRE(BigInteger(5) == BigInteger("0000005"));
    REQUIRE(BigInteger(10) == BigInteger("a"));
    
    REQUIRE_FALSE(BigInteger(10) == BigInteger(11));
}

TEST_CASE("CanDetectInequality")
{
    REQUIRE(BigInteger(5) != BigInteger(4));
    REQUIRE(BigInteger(5) != BigInteger("0000006"));
    REQUIRE(BigInteger(3) != BigInteger("0000005"));
    REQUIRE(BigInteger(10) != BigInteger("10"));
    
    REQUIRE_FALSE(BigInteger(10) != BigInteger(10));
}

TEST_CASE("CanDetectLessThan")
{
    REQUIRE(BigInteger(9) < BigInteger(10));
    REQUIRE_FALSE(BigInteger(10) < BigInteger(10));
    REQUIRE_FALSE(BigInteger(11) < BigInteger(10));
}

TEST_CASE("CanDetectLessThanOrEqualTo")
{
    REQUIRE(BigInteger(9) <= BigInteger(10));
    REQUIRE(BigInteger(10) <= BigInteger(10));
    REQUIRE_FALSE(BigInteger(11) <= BigInteger(10));
}

TEST_CASE("CanDetectGreaterThan")
{
    REQUIRE_FALSE(BigInteger(9) > BigInteger(10));
    REQUIRE_FALSE(BigInteger(10) > BigInteger(10));
    REQUIRE(BigInteger(11) > BigInteger(10));
}

TEST_CASE("CanDetectGreaterThanOrEqualTo")
{
    REQUIRE_FALSE(BigInteger(9) >= BigInteger(10));
    REQUIRE(BigInteger(10) >= BigInteger(10));
    REQUIRE(BigInteger(11) >= BigInteger(10));
}

TEST_CASE("CanMultiplySmallNumbers")
{
    REQUIRE((BigInteger(1) * BigInteger(1)).ToString() == "01");
    REQUIRE((BigInteger(1) * BigInteger(2)).ToString() == "02");
    REQUIRE((BigInteger(1) * BigInteger(3)).ToString() == "03");
}

TEST_CASE("CanAddLargerNumbers")
{
    AddTwoNumbersTest(0xf, 0xf);
    AddTwoNumbersTest(0xff, 0xff);
    AddTwoNumbersTest(0xfff, 0xfff);
    AddTwoNumbersTest(0xffff, 0xffff);
}

TEST_CASE("SpecificAdditionTest")
{
    AddTwoNumbersTest(0x798c6, 0x34);
}

TEST_CASE("StatisticalAdditionTest")
{
    srand(static_cast<unsigned int>(time(nullptr)));
    for(int i = 0; i < 1000; i++)
    {
        AddTwoNumbersTest(rand() % 0xfffff, rand() % 0xfffff, i);
    }
}

TEST_CASE("CanMultiplyLargerNumbers")
{
    REQUIRE((BigInteger(1000) * BigInteger(50)).ToString() == "c350");
    //REQUIRE((BigInteger(0xffffffff) * BigInteger(0xffffffff)).ToString() == "1fffffffe");
    //REQUIRE((BigInteger(1) * BigInteger(3)).ToString() == "03");
}












