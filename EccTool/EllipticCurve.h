//
//  EllipticCurve.h
//  EccTool
//
//  Created by Josh Strom on 1/25/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__EllipticCurve__
#define __EccTool__EllipticCurve__

#include <iostream>
#include <string>
#include "BigInteger.h"
#include "EccDefs.h"

using namespace std;
using namespace ecc;

//BigInteger ModularMultiply(BigInteger& rhs)

class EllipticCurve
{
    // --
    // Members/functions of EllipticCurve
    // --
private:
    const char* COMPRESSED_GENERATOR_FLAG = "02";
    const char* UNCOMPRESSED_GENERATOR_FLAG = "04";
    
    // The parameters for this particular curve.
    // The field Fp over which the equation operates.
    BigInteger _p;
    
    // The coefficients which define the curve.
    BigInteger _a;
    BigInteger _b;

    // The Generator or Base Point of this curve.
    Point _G;
    
    // The order of the curve generator point G.
    BigInteger _n;
    
    // The cofactor of the curve.
    BigInteger _h;
    
    // Helper functions for parsing the Generator point from the domain parameters.
    Point ParseGeneratorPoint(DomainParameters domainParams) const;
    Point ParseUncompressedGeneratorPoint(const string& pointString) const;
    Point ParseCompressedGeneratorPoint(const string& pointString)const;
    
    // Internal functions to compute the addition of two points for
    //  a) A + B = C (when A and B are distinct), and
    //  b) A + A = C (when A is added to itself)
    Point PointAdd(const Point& rhs, const Point& lhs) const;
    Point PointDouble(const Point& point) const;
    
    // Internal helper functions for performing arithmetic over the finite field defined
    //  in the curve.
    BigInteger AddInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const;
    BigInteger SubtractInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const;
    BigInteger MultiplyInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const;
    BigInteger DivideInFiniteField(const BigInteger& numerator, const BigInteger& denominator) const;

    // Uses the Extended Euclidian Algorithm to calculate the multiplicative inverse of
    //  'a' in the finite field 'b'.
    static BigInteger FindMultiplicativeInverse(const BigInteger& a, const BigInteger& b);
    
public:
    // Point at infinity.
    static const Point O;
    
    // Initializes a curve with the supplied parameters.
    EllipticCurve(DomainParameters params);
    
    // Adds two points on the given curve. Both points must be on the curve (results are undefined otherwise).
    Point AddPointsOnCurve(const Point& rhs, const Point& lhs) const;
    
    // Inverts the given point on the curve. Point must be on the curve (results are undefined otherwise).
    Point InvertPoint(const Point& point) const;
    
    // Multiplies the given point on the curve with the given scalar. Point must be on the curve (result
    //  are undefined otherwise).
    Point MultiplyPointOnCurveWithScalar(const Point& point, const BigInteger& scalar) const;
    
    // Returns true|false depending on whether the given point is on the curve.
    bool CheckPointOnCurve(const Point& point) const;
    
    // Returns the Generator, or Base Point of this curve.
    const Point& GetBasePoint() const;
};

#endif /* defined(__EccTool__EllipticCurve__) */
