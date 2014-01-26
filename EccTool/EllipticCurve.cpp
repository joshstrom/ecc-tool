//
//  EllipticCurve.cpp
//  EccTool
//
//  Created by Josh Strom on 1/25/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "EllipticCurve.h"
#include "EccDefs.h"
#include <string>
#include <exception>
#include <iostream>

using namespace std;
using namespace ecc;

const Point EllipticCurve::O = Point::MakePointAtInfinity();

EllipticCurve::EllipticCurve(DomainParameters params) : _h(params.h), _n(params.n), _p(params.p), _a(params.a), _b(params.b), _G(ParseGeneratorPoint(params))
{
    // Validate that the base point G is on the curve.
    if(!CheckPointOnCurve(_G))
    {
        throw invalid_argument("Invalid curve parameters: Generator point not on curve.");
    }
}

Point EllipticCurve::ParseGeneratorPoint(DomainParameters params) const
{
    // Strip out any spaces in the generator.
    string point(params.G);
    point.erase(std::remove_if(point.begin(), point.end(), ::isspace), point.end());
    
    if(point.size() < 2)
        throw invalid_argument("Valid generator not provided: buffer too short.");
    
    // Pull the first two characters from the beginning of the string, this indicates whether the point is compressed
    //  or uncompressed.
    string type = point.substr(0, 2);
    if(type == COMPRESSED_GENERATOR_FLAG)
        return ParseCompressedGeneratorPoint(point.substr(2, string::npos));
    if(type == UNCOMPRESSED_GENERATOR_FLAG)
        return ParseUncompressedGeneratorPoint(point.substr(2, string::npos));
    
    throw invalid_argument("Invalid generator point compression flag.");
}

Point EllipticCurve::ParseUncompressedGeneratorPoint(const string& pointString) const
{
    // The first half of the point is the x-coordinate, the second half of the point is the y-coordinate.
    if((pointString.size() % 2) != 0)
        throw invalid_argument("Generator point not serialized correctly: not same size.");
    
    // Create the point: x = first half of point string, y = second half of point string.
    return Point({BigInteger(string(pointString.begin(), pointString.begin() + (pointString.size() / 2))),
        BigInteger(string(pointString.begin() + (pointString.size() / 2), pointString.end()))});
}

Point EllipticCurve::ParseCompressedGeneratorPoint(const string& pointString)const
{
    throw invalid_argument("compressed generator point parsing not implemented.");
}

Point EllipticCurve::PointAdd(const Point& P, const Point& Q) const
{
    
    // Compute the formula to for point adding (used in cases where the points to be added are not the same):
    //  s = (P:y - Q:y) / (P:x - Q:x)
    //  r:x = s^2 - P:x - Q:x
    //  r:y = s(P:x - R:x) - (P:y)
    // Where:
    //  s = computed slope of the line.
    //  P:x|y, Q:x|y = the x and y coordinates of the given points P and Q.
    //  r:x|y = the newly computed point x and y coordinates.
    //  a = one of the coefficients of the curve.
    //  All calculations are done mod p (where p is the finite field of the curve).
    
    BigInteger s = DivideInFiniteField(SubtractInFiniteField(P.y, Q.y), SubtractInFiniteField(P.x, Q.x));
    BigInteger Rx = SubtractInFiniteField(SubtractInFiniteField(MultiplyInFiniteField(s, s), P.x), Q.x);
    BigInteger Ry = SubtractInFiniteField(MultiplyInFiniteField(s, SubtractInFiniteField(P.x, Rx)), P.y);
    
    return Point({Rx, Ry});
}

Point EllipticCurve::PointDouble(const Point& P) const
{
    // Compute the formula to for point doubling (used in cases where the points to be added ARE the same):
    //  s = (3(P:x)^2 + a) / (2(P:y))
    //  r:x = s^2 - 2(P:x)
    //  r:y = s(P:x - R:x) - P:y
    // Where:
    //  s = computed slope of the line.
    //  P:x|y = the x and y coordinates of the given point.
    //  r:x|y = the newly computed point x and y coordinates.
    //  a = one of the coefficients of the curve.
    //  All calculations are done mod p (where p is the finite field of the curve) using the
    //      <operation>InFiniteField() functions.
    
    BigInteger s = DivideInFiniteField(AddInFiniteField(MultiplyInFiniteField(3, MultiplyInFiniteField(P.x, P.x)), _a), MultiplyInFiniteField(2, P.y));
    BigInteger Rx = SubtractInFiniteField(MultiplyInFiniteField(s, s), MultiplyInFiniteField(2, P.x));
    BigInteger Ry = SubtractInFiniteField(MultiplyInFiniteField(s, SubtractInFiniteField(P.x, Rx)), P.y);
    
    return Point({Rx, Ry});
}

Point EllipticCurve::InvertPoint(const Point& point) const
{
    // The inverse of a point on the curve is defined as:
    //  -{x,y} <=> {x,-y} (subtraction done in finite field).
    return Point(point.x, SubtractInFiniteField(0,point.y));
}

Point EllipticCurve::AddPointsOnCurve(const Point& rhs, const Point& lhs) const
{
    // Special rules O (the point at infinity)
    // For point P and point at infinity O:
    //  P + (-P) = O    (and since addition is communitive: (-P) + P = O)
    //  P + O = P       (and since addition is communitive: O + P = P)
    if(rhs == O)
        return lhs;
    if(lhs == O)
        return rhs;
    if(rhs == InvertPoint(lhs))
        return O;
    
    // For points P and Q, P+Q is added differently (point add) than P+P (point double).
    if(rhs == lhs)
        return PointDouble(rhs);
    else
        return PointAdd(rhs, lhs);
}

bool EllipticCurve::CheckPointOnCurve(const Point& point) const
{
    // For the point (x,y) to be on the curve,
    //  y^2 MUST EQUAL x^3 + ax + b    mod p
    BigInteger leftHandSide = MultiplyInFiniteField(point.y, point.y);
    BigInteger rightHandSide = AddInFiniteField(AddInFiniteField(MultiplyInFiniteField(MultiplyInFiniteField(point.x, point.x), point.x), MultiplyInFiniteField(_a, point.x)),_b);
    bool pointIsOnCurve =  rightHandSide == leftHandSide;
    
    return pointIsOnCurve;
}

const Point& EllipticCurve::GetBasePoint() const
{
    return _G;
}

BigInteger EllipticCurve::AddInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const
{
    // Let p define the max of the fininte field Fp such that all elemnts of Fp are in the range
    //  [0, p-1].
    // Since both operands are in Fp, the result of an addition operation must be
    //  in the range [0, 2p-1]. Thus, to ensure that the result of the addition is an element of Fp,
    //  do the following transformation:
    //      case (result < p): return result.
    //      case (result > p): return result - p (which will place it back in the range [0, p-1].
    BigInteger result = operand1 + operand2;
    if(result >= _p)
        result -= _p;
    
    return result;
}

BigInteger EllipticCurve::SubtractInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const
{
    // Let p define the max of the fininte field Fp such that all elemnts of Fp are in the range
    //  [0, p-1].
    // Since both operands are in Fp, the result of a subtraction operation must be
    //  in the range [-(p-1), p-1]. Thus, to ensure that the result of the subtraction is an element of Fp,
    //  do the following transformation:
    //      case (result >= 0): return result.
    //      case (result < p): return result + p (which will place it back in the range [0, p-1].
    BigInteger result = operand1 - operand2;
    if(result < 0)
        result += _p;
    
    return result;
}

BigInteger EllipticCurve::MultiplyInFiniteField(const BigInteger& operand1, const BigInteger& operand2) const
{
    // Let p define the max of the fininte field Fp such that all elemnts of Fp are in the range
    //  [0, p-1].
    // To ensure that the result of the multiplication operation is an element of Fp, return the result
    //  mod p, which will place the result in the range [0, p-1]
    BigInteger result = operand1 * operand2;
    return result %= _p;
}

BigInteger EllipticCurve::DivideInFiniteField(const BigInteger& numerator, const BigInteger& denominator) const
{
    // Let p define the max of the fininte field Fp such that all elemnts of Fp are in the range
    //  [0, p-1].
    //  Divide numerator by denominator by multiplying the numerator by the multiplicative inverse
    //  of the numerator, mod p.
    return MultiplyInFiniteField(numerator, EllipticCurve::FindMultiplicativeInverse(denominator, _p));
}

BigInteger EllipticCurve::FindMultiplicativeInverse(const BigInteger& a, const BigInteger& b)
{
    // Use the following algorithm to calculate the multiplicative inverese via the
    //  Extended Euclidian Algorithm:
    //    function eea(a, b)
    //        s := 0;    old_s := 1
    //        t := 1;    old_t := 0
    //        r := b;    old_r := a
    //        while r ≠ 0
    //            quotient := old_r div r
    //            (old_r, r) := (r, old_r - quotient *r)
    //            (old_s, s) := (s, old_s - quotient *s)
    //            (old_t, t) := (t, old_t - quotient *t)
    //        output "Bézout coefficients:", (old_s, old_t)
    //        output "greatest common divisor:", old_r
    //        output "quotients by the gcd:", (t, s)
    
    //    Where (old_r, r) := (r, old_r - quotient *r) means:
    //        prov := r;
    //        r := old_r - quotient * prov;
    //        old_r := prov;

    BigInteger s = 0;
    BigInteger old_s = 1;
    
    BigInteger t = 1;
    BigInteger old_t = 0;
    
    BigInteger r = b;
    BigInteger old_r = a;
    
    while(r != 0)
    {
        BigInteger quotient = old_r / r;
        
        BigInteger prov = r;
        r = old_r - (quotient * prov);
        swap(old_r, prov);
        
        prov = s;
        s = old_s - (quotient * prov);
        swap(old_s, prov);
        
        prov = t;
        t = old_t - (quotient * prov);
        swap(old_t, prov);
    }

    // old_s is the multiplicative inverse of a, but may be a negative number.
    //  Place this within Fp by adding p to the result (if the result is less than 0).
    if(old_s < 0)
        old_s += b;
    
    return old_s;
}



