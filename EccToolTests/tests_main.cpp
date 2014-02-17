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
#include "EllipticCurve.h"
#include "DefinedCurveDomainParameters.h"
#include "FieldElement.h"
#include "Utilities.h"
#include "Version1KeySerializer.h"
#include "NativeCrypto.h"

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
    
    return static_cast<unsigned long>(sw.GetElapsedTime() / iterationCount);
}

TEST_CASE("CanCreateBigIntegerWithHexString")
{
    BigInteger one("1");
    REQUIRE("01" == one.ToString());
}

TEST_CASE("CanCreateFromStringWithSpaces")
{
    BigInteger bigIntegerWithSpaces("1234 5678");
    REQUIRE("12345678" == bigIntegerWithSpaces.ToString());
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

TEST_CASE("CanCompareToZeroWithTemplate")
{
    REQUIRE(BigInteger(1) != 0);
    REQUIRE(BigInteger(0) == 0);
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
    RunSubtractionTest(0x2c5e, 0x0eac);
}

TEST_CASE("SpecificDivisionTest")
{
	RunDivisionTest(14, 5);
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

TEST_CASE("CanDivideSmallNumberByLargerOne")
{
    REQUIRE((BigInteger(2) / BigInteger(5)).ToString() == "00");
    REQUIRE((BigInteger(2) % BigInteger(5)).ToString() == "02");
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

TEST_CASE("CanAddInFiniteField")
{
    BigInteger addend(5);
    BigInteger field(6);
    //ModularAdd(addend, addend, field);
}

TEST_CASE("CanCreateEllipticCurveWithParameters_secp112r1")
{
    DomainParameters params = {
            "secp112r1", //name
            "DB7C 2ABF62E3 5E668076 BEAD208B", //p
            "DB7C 2ABF62E3 5E668076 BEAD2088", //a
            "659E F8BA0439 16EEDE89 11702B22", //b
            "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
            "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
            "01" //h
       };
    
    EllipticCurve curve(params);
    const Point& generatorPoint = curve.GetBasePoint();
    Point sum = curve.AddPointsOnCurve(generatorPoint, generatorPoint);
    
    BigInteger expectedX("57cf52a0f9318000ee0bc032d756");
    BigInteger expectedY("60aee03bbcff537a8d17401f006c");

    REQUIRE(sum.x == expectedX);
    REQUIRE(sum.y == expectedY);
    
    REQUIRE(curve.CheckPointOnCurve(sum));
    
    Point sum2 = curve.AddPointsOnCurve(generatorPoint, sum);
    REQUIRE(sum != sum2);
    REQUIRE(curve.CheckPointOnCurve(sum2));
    
}

TEST_CASE("CanInvertPoint")
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    EllipticCurve curve(params);
    
    Point basePoint = curve.GetBasePoint();
    Point invertedBasePoint = curve.InvertPoint(basePoint);
    
    REQUIRE(basePoint.x == invertedBasePoint.x);
    REQUIRE(basePoint.y != invertedBasePoint.y);
    
    Point doubleInvertedBasePoint = curve.InvertPoint(invertedBasePoint);
    REQUIRE(invertedBasePoint != doubleInvertedBasePoint);
    REQUIRE(basePoint == doubleInvertedBasePoint);
}

TEST_CASE("AddinWithPointAtInfinity")
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    EllipticCurve curve(params);
    
    Point basePoint = curve.GetBasePoint();
    
    // Adding a point on curve to infinity.
    REQUIRE(basePoint == curve.AddPointsOnCurve(basePoint, EllipticCurve::PointAtInfinity));
    // Adding infinity to a point on the curve.
    REQUIRE(basePoint == curve.AddPointsOnCurve(EllipticCurve::PointAtInfinity, basePoint));
    
    // Adding infinity to itself.
    REQUIRE(EllipticCurve::PointAtInfinity == curve.AddPointsOnCurve(EllipticCurve::PointAtInfinity, EllipticCurve::PointAtInfinity));
    
    // Add a point on the curve to its inverse.
    REQUIRE(EllipticCurve::PointAtInfinity == curve.AddPointsOnCurve(basePoint, curve.InvertPoint(basePoint)));
    // Add a an inverse of a point on the curve to that point on the curve.
    REQUIRE(EllipticCurve::PointAtInfinity == curve.AddPointsOnCurve(curve.InvertPoint(basePoint), basePoint));
}

TEST_CASE("MultiplyWithScalar")
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    EllipticCurve curve(params);
    
    BigInteger scalar = 5;
    Point basePoint = curve.GetBasePoint();
    
    Point expectedPoint = curve.MakePointOnCurve(BigInteger("ca188ca33fde3ce02a83f197547c"), BigInteger("ab5c59cb715e2ddc3e51ad252a5f"));
    
    Point multipliedPoint = curve.MultiplyPointOnCurveWithScalar(basePoint, scalar);
    
    REQUIRE(multipliedPoint != basePoint);
    REQUIRE(curve.CheckPointOnCurve(multipliedPoint));
    REQUIRE(expectedPoint == multipliedPoint);
 
}

TEST_CASE("EccAlgGenerateEccKeypair")
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    EllipticCurve curve(params);
    
    EccAlg alg(curve);
    REQUIRE_NOTHROW(alg.GenerateKeys());
}

TEST_CASE("EccAlgCanGenerateAndPersistKeys")
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    EllipticCurve curve(params);
    
    EccAlg alg(curve);
    alg.GenerateKeys();
}

TEST_CASE("CanGetCurveByName")
{
    const string curveName = "secp256k1";
    DomainParameters params = ecc::GetCurveByName(curveName);
    
    REQUIRE(params.name == curveName);
}

TEST_CASE("CanMakeBitcoinCurve")
{
    DomainParameters bitcoinCurveParams = GetSecp256k1Curve();
    EllipticCurve curve(bitcoinCurveParams);
    
    EccAlg alg(curve);
    alg.GenerateKeys();
}

TEST_CASE("CanCreateFieldElement")
{
    FieldElement element(5, 7);
}

TEST_CASE("CanAddFieldElementsResultOutOfField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(5, p);
    FieldElement rhs(4, p);
    
    REQUIRE((lhs + rhs) == 2);
}

TEST_CASE("CanSubtractFieldElementsResultOutOfField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(4, p);
    FieldElement rhs(6, p);
    
    REQUIRE((lhs - rhs) == 5);
}

TEST_CASE("CanMultiplyFieldElementsResultOutOfField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(5, p);
    FieldElement rhs(5, p);
    
    REQUIRE((lhs * rhs) == 4);
}

TEST_CASE("CanDivideFieldElements")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(4, p);
    FieldElement rhs(2, p);
    
    REQUIRE((lhs / rhs) == 2);
    REQUIRE((rhs / lhs) == 4);
}

TEST_CASE("CanAddFieldElementsResultInField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(2, p);
    FieldElement rhs(2, p);
    
    REQUIRE((lhs + rhs) == 4);
}

TEST_CASE("CanSubtractFieldElementsResultInField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(4, p);
    FieldElement rhs(2, p);
    
    REQUIRE((lhs - rhs) == 2);
}

TEST_CASE("CanMultiplyFieldElementsResultInField")
{
    auto p = make_shared<BigInteger>(7);
    FieldElement lhs(2, p);
    FieldElement rhs(2, p);
    
    REQUIRE((lhs * rhs) == 4);
}

TEST_CASE("CanSerializeAndDeserializePoint")
{
    const string curveName = "secp112r1";
    DomainParameters params = ecc::GetCurveByName(curveName);
    
    auto p = make_shared<BigInteger>(params.p);

    // Parse the generator point. Serialize the point, Parse it again. It should be the same.
    Point parsed = Point::Parse(utilities::HexStringToBytes(params.G), p);
    auto serialized = parsed.Serialize();
    
    
    Point parsedAgain = Point::Parse(serialized, p);
    
    REQUIRE(parsed == parsedAgain);
}

TEST_CASE("CanDeserializePointWithExtraBytesAppended")
{
    uint8_t serializedPoint[] { 0x04, 0x01, 0x02, 0x00, 0x00 }; // Serialized point, two zero bytes added.
    BigInteger field(9);
    
    auto parsed = Point::Parse(vector<uint8_t>(serializedPoint, serializedPoint + sizeof(serializedPoint)), make_shared<BigInteger>(field));
    
    REQUIRE(parsed.x == 1);
    REQUIRE(parsed.y == 2);
}

TEST_CASE("CanParseHexString")
{
    uint8_t expectedArr[] = { 0x1, 0x2, 0x3, 0x4 };
    vector<uint8_t> expected(expectedArr, expectedArr + sizeof(expectedArr));
    
    auto parsed = utilities::HexStringToBytes("01020304");
    
    REQUIRE(expected == parsed);
}

TEST_CASE("CanParseHexStringMissingLeadingZero")
{
    uint8_t expectedArr[] = { 0x1, 0x2, 0x3, 0x4 };
    vector<uint8_t> expected(expectedArr, expectedArr + sizeof(expectedArr));
    
    auto parsed = utilities::HexStringToBytes("1020304");
    
    REQUIRE(expected == parsed);
}

TEST_CASE("CanParseHexStringWithSpaces")
{
    uint8_t expectedArr[] = { 0x1, 0x2, 0x3, 0x4 };
    vector<uint8_t> expected(expectedArr, expectedArr + sizeof(expectedArr));
    
    auto parsed = utilities::HexStringToBytes("01 02 03 04");
    
    REQUIRE(expected == parsed);
}

TEST_CASE("CanCreatePositiveBigIntegerFromBytes")
{
    uint8_t bytesArr[] = { 0x1, 0x2, 0x3, 0x4 };
    vector<uint8_t> bytes(bytesArr, bytesArr + sizeof(bytesArr));
    
    BigInteger integer(bytes);
    
    REQUIRE(BigInteger("01020304") == integer);
}

TEST_CASE("CanCreateNegativeBigIntegerFromBytes")
{
    uint8_t bytesArr[] = { 0x1, 0x2, 0x3, 0x4 };
    vector<uint8_t> bytes(bytesArr, bytesArr + sizeof(bytesArr));
    
    BigInteger integer(bytes, false);
    
    REQUIRE(BigInteger("-01020304") == integer);
}

TEST_CASE("CanSerializeAndParseBigInteger")
{
    BigInteger first("01020304");
    auto serialized = first.GetMagnitudeBytes();
    BigInteger second(serialized);
    
    REQUIRE(first == second);
}

TEST_CASE("CanSerializeAndParsePrivateKeys")
{
    DomainParameters curveParams = GetSecp112r1Curve();
    EllipticCurve curve(GetSecp112r1Curve());
    
    EccAlg alg1(curve);
    alg1.GenerateKeys();
    
    auto pubKey = alg1.GetPublicKey();
    auto privKey = alg1.GetPrivateKey();
    
    EccAlg alg2(curve);
    REQUIRE_NOTHROW(alg2.SetKey(pubKey, privKey));

}

TEST_CASE("CanLoadV1SerializedKeys")
{
    DomainParameters curveParams = GetSecp112r1Curve();
    EllipticCurve curve(GetSecp112r1Curve());

    EccAlg alg1(curve);
    alg1.GenerateKeys();
    
    auto serializedKeys = Version1KeySerializer().SerializePrivateKeys(alg1);
    
    REQUIRE_NOTHROW(auto alg2 = Version1KeySerializer().ParseKeys(serializedKeys));
    
}

TEST_CASE("CanSerializeAndParsePublicKey")
{
    DomainParameters curveParams = GetSecp112r1Curve();
    EllipticCurve curve(GetSecp112r1Curve());
    
    EccAlg alg1(curve);
    alg1.GenerateKeys();
    
    auto serializedKey = Version1KeySerializer().SerializePublicKeys(alg1);
    
    EccAlg alg2 = Version1KeySerializer().ParseKeys(serializedKey);
    REQUIRE_THROWS(alg2.GetPrivateKey());
}

TEST_CASE("CanDeriveKey")
{
    vector<uint8_t> password(10);
    auto key = NativeCrypto::DeriveKey(password, password, 32);
}

TEST_CASE("CanEncrypt")
{
    DomainParameters curveParams = GetSecp112r1Curve();
    EllipticCurve curve(GetSecp112r1Curve());
    
    EccAlg alg1(curve);
    alg1.GenerateKeys();
    
    uint8_t messageArr[] = { 0, 1, 2, 3, 4, 5 };
    vector<uint8_t> message(messageArr, messageArr + sizeof(messageArr));
    
    auto encrypted = alg1.Encrypt(message);
    auto decrypted = alg1.Decrypt(encrypted);
    
    REQUIRE(decrypted == message);
}






























