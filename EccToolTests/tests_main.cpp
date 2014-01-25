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
#include "OperationTesters.h"
#include "Stopwatch.h"

void StatisticalOperationTest(const BaseOperationTester& tester)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    for(int i = 0; i < 10000; i++)
    {
        tester.TestOperation(rand() % 0xffffff, rand() % 0xffffff, nullptr, i);
    }
}

void StatisticalOperationTestAllowNegatives(const BaseOperationTester& tester)
{
	srand(static_cast<unsigned int>(time(nullptr)));
    for(int i = 0; i < 10000; i++)
    {
        bool firstOperandNegative = ((rand() % 2) % 2) == 0;
        bool secondOperandNegative = ((rand() % 2) % 2) == 0;
        
        int firstOperandSign = (firstOperandNegative) ? -1 : 1;
        int secondOperandSign = (secondOperandNegative) ? -1 : 1;
        
        tester.TestOperation((rand() % 0xffffff) * firstOperandSign, (rand() % 0xffffff) * secondOperandSign, nullptr, i);
    }
}

unsigned long RunOperationPerformanceTest(const BaseOperationTester& tester, int operand1, int operand2)
{
    Stopwatch sw;
    
    int iterationCount = 10000;
    for(int i = 0; i < iterationCount; i++)
    {
        tester.TestOperation(operand1, operand2, &sw);
    }
    
    return sw.GetElapsedTime() / iterationCount;
}

TEST_CASE("CanCreateBigIntegerWithHexString")
{
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateWithNegativeInteger")
{
    BigInteger negativeInteger(-3);
    REQUIRE(negativeInteger < 0);
}

TEST_CASE("CanCreateWithNegativeIntegerString")
{
    BigInteger negativeInteger("-3");
    REQUIRE(negativeInteger == -3);
}

TEST_CASE("CanNegate")
{
    BigInteger postive(5);
    BigInteger negative(-5);
    BigInteger zero(0);
    
    REQUIRE((-postive) == negative);
    REQUIRE((-negative) == postive);
    REQUIRE((-zero) == zero);
}

TEST_CASE("CorrectlyComputesAbsoluteValue")
{
    REQUIRE(abs(BigInteger(5)) == BigInteger(5));
    REQUIRE(abs(BigInteger(0)) == BigInteger(0));
    REQUIRE(abs(BigInteger(-5)) == BigInteger(5));
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
    RunAdditionTest(0xf, 0xf);
    RunAdditionTest(0xff, 0xff);
    RunAdditionTest(0xfff, 0xfff);
    RunAdditionTest(0xffff, 0xffff);
}

TEST_CASE("CanAddTwoNegativeNumbers")
{
    RunAdditionTest(-5, -5);
}

TEST_CASE("CanAddLargerNegativeToSmallerPositive")
{
	RunAdditionTest(-5, 3);
}

TEST_CASE("CanAddLargerPositiveToSmallerNegative")
{
	RunAdditionTest(5, -3);
}

TEST_CASE("CanAddSmallerNegativeToLargerPositive")
{
	RunAdditionTest(-3, 5);
}

TEST_CASE("CanAddSmallerPositiveToLargerNegative")
{
	RunAdditionTest(3, -5);
}

TEST_CASE("CanAddPositiveToInverse")
{
	RunAdditionTest(5, -5);
}

TEST_CASE("CanAddNegativeToInverse")
{
	RunAdditionTest(-5, 5);
}

TEST_CASE("CanSubtractWithNegatives")
{
	REQUIRE((BigInteger(-2) - BigInteger(-3)).ToString() == BigInteger(1).ToString());
	REQUIRE((BigInteger(2) - BigInteger(1)).ToString() == BigInteger(1).ToString());
	REQUIRE((BigInteger(1) - BigInteger(2)).ToString() == BigInteger(-1).ToString());
}

TEST_CASE("CanMultiplyWithNegative")
{
	RunMultiplicationTest(2, 2);
	RunMultiplicationTest(-2, 2);
	RunMultiplicationTest(2, -2);
	RunMultiplicationTest(-2, -2);
}

TEST_CASE("StatisticalMultiplicationTest")
{
    MultiplicationOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("StatisticalAdditionTest")
{
    AdditionOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("StatisticalSubtractionTester")
{
    SubtractionOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("StatisticalComparisonTest")
{
    AllComparisonOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("SpecificMultiplicationTest")
{
    RunMultiplicationTest(0xcc437, 0x131ce);
}

TEST_CASE("SpecificAdditionTest")
{
    RunAdditionTest(0x7682f, 0xcb00);
}

TEST_CASE("SpecificSubtractionTest")
{
    RunSubtractionTest(0x79158, 0x5e);
}

TEST_CASE("CanShiftWholeBytes8")
{
    BigInteger originalInteger("2");
    BigInteger shifted("0200");
    
    originalInteger <<= 8;
    
    REQUIRE(shifted.ToString() == originalInteger.ToString());
}

TEST_CASE("CanShiftWholeBytes16")
{
    BigInteger originalInteger("2");
    BigInteger shifted("020000");
    
    originalInteger <<= 16;
    
    REQUIRE(shifted.ToString() == originalInteger.ToString());
}

TEST_CASE("CanShiftFractionalBytes")
{
    BigInteger originalInteger("01020304");
    BigInteger shifted("10203040");
    
    originalInteger <<= 4;
    
    REQUIRE(shifted.ToString() == originalInteger.ToString());
}

TEST_CASE("CanShiftBothWholeAndFractionalBytes")
{
    BigInteger originalInteger("FFFFFFFF");
    BigInteger shifted("1FFFFFFFE000");
    
    originalInteger <<= 13;
    
    REQUIRE(shifted.ToString() == originalInteger.ToString());
}

TEST_CASE("CanShiftSingleBitThirteenBits")
{
    BigInteger originalInteger("1");
    BigInteger shifted("2000");
    
    originalInteger <<= 13;
    
    REQUIRE(shifted.ToString() == originalInteger.ToString());
}

TEST_CASE("CanTakeModuloOfSmallNumbers")
{
    REQUIRE((BigInteger(5) % BigInteger(8)) == 5);
    REQUIRE((BigInteger(8) % BigInteger(5)) == 3);
    REQUIRE((BigInteger(-8) % BigInteger(5)) == 3);
}

TEST_CASE("CanTakeModuloOfVeryLargeNumbers")
{
    BigInteger veryLargeNumber1("0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
    BigInteger veryLargeNumber2("0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28");
    
    BigInteger modulo = veryLargeNumber1 % veryLargeNumber2;
    REQUIRE(modulo >= 0);
    REQUIRE(modulo < veryLargeNumber1);
}

TEST_CASE("CanSetAndGetBitsInFirstByte")
{
    BigInteger integer(0);
    REQUIRE(integer.GetBitSize() == 8);
    REQUIRE(integer.GetBitAt(4) == false);
    
    integer.SetBitAt(4);
    REQUIRE(integer.GetBitAt(4) == true);
    REQUIRE(integer.ToString() == "10");
    
    integer.ClearBitAt(4);
    REQUIRE(integer.GetBitAt(4) == false);
    REQUIRE(integer.ToString() == "00");
}

TEST_CASE("CanSetAndGetBitsInSecondByte")
{
    BigInteger integer(0x8000);
    REQUIRE(integer.GetBitSize() == 16);
    REQUIRE(integer.GetBitAt(9) == false);
    
    integer.SetBitAt(9);
    REQUIRE(integer.GetBitAt(9) == true);
    REQUIRE(integer.ToString() == "8200");
    
    integer.ClearBitAt(9);
    REQUIRE(integer.GetBitAt(9) == false);
    REQUIRE(integer.ToString() == "8000");
}

TEST_CASE("CanDividePositiveNumbers")
{
    RunDivisionTest(10, 2);
    RunDivisionTest(5, 10);
}

TEST_CASE("StatisticalDivisionTest")
{
    DivisionOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("StatisticalModulusTest")
{
    ModulusOperationTester tester;
    StatisticalOperationTestAllowNegatives(tester);
}

TEST_CASE("CanDivideNegativeNumbers")
{
    RunDivisionTest(-10, 2);
    RunDivisionTest(10, -2);
    RunDivisionTest(-10, -2);
}

TEST_CASE("AdditionPerformanceTest")
{
    AdditionOperationTester tester;
    RunOperationPerformanceTest(tester, 5, 5);
}
















