//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Joshua Strom
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
#include "EllipticCurve.h"
#include "EccDefs.h"
#include "FieldElement.h"
#include "Utilities.h"
#include <string>
#include <exception>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace ecc;

const Point EllipticCurve::PointAtInfinity = Point::MakePointAtInfinity();
const char* EllipticCurve::COMPRESSED_GENERATOR_FLAG = "02";
const char* EllipticCurve::UNCOMPRESSED_GENERATOR_FLAG = "04";

EllipticCurve::EllipticCurve(DomainParameters params) : _p(make_shared<BigInteger>(params.p)), _a(params.a, _p), _b(params.b, _p), _G(Point::Parse(utilities::HexStringToBytes(params.G), 0, _p)), _n(params.n), _h(params.h), _curveName(params.name)
{
    // Validate that the base point G is on the curve.
    if(!CheckPointOnCurve(_G))
        throw invalid_argument("Invalid curve parameters: Generator point not on curve.");
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

    FieldElement s = (P.y - Q.y) / (P.x - Q.x);
    FieldElement Rx = (s * s) - P.x - Q.x;
    FieldElement Ry = (s * (P.x - Rx)) - P.y;
    
    return Point(move(Rx), move(Ry));
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
    
    FieldElement s = ((FieldElement(3, _p) * (P.x * P.x)) + _a) / (FieldElement(2, _p) * P.y);
    FieldElement Rx = (s * s) - (FieldElement(2, _p) * P.x);
    FieldElement Ry = (s * (P.x - Rx)) - P.y;
    
    return Point(move(Rx), move(Ry));
}

Point EllipticCurve::InvertPoint(const Point& point) const
{
    // The inverse of a point on the curve is defined as:
    //  -{x,y} <=> {x,-y} (subtraction done in finite field).
    return Point(point.x, -point.y);
}

Point EllipticCurve::AddPointsOnCurve(const Point& rhs, const Point& lhs) const
{
    // Special rules O (the point at infinity)
    // For point P and point at infinity O:
    //  P + (-P) = O    (and since addition is communicative: (-P) + P = O)
    //  P + O = P       (and since addition is communicative: O + P = P)
    if(rhs == PointAtInfinity)
        return lhs;
    if(lhs == PointAtInfinity)
        return rhs;
    if(rhs == InvertPoint(lhs))
        return PointAtInfinity;
    
    // For points P and Q, P+Q is added differently (point add) than P+P (point double).
    if(rhs == lhs)
        return PointDouble(rhs);
    else
        return PointAdd(rhs, lhs);
}

// Multiplies the given point on the curve with the given scalar. Point must be on the curve (result
//  are undefined otherwise).
Point EllipticCurve::MultiplyPointOnCurveWithScalar(const Point& point, const BigInteger& scalar) const
{
    // Multiplies according the the below algorithm (found here: http://www-ma2.upc.es/~cripto/Q2-06-07/ECC_eng.pdf):
    //function multiplyByScalar(point A, scalar k)
    //    N = A
    //    R = O    // Point at infinity
    //    r = <Most Significat Bit b of k | b != 0>
    //    Iterate k from r to 0 (current bit designated by i) {
    //        R = R + R
    //        if (bit_is_set(i)) {
    //            R = R + P
    //        }
    //    }
    //    output R
    
    // This algorithm is significantly more efficient than repeated addition because of the huge size
    // of some of these numbers.

    Point product = EllipticCurve::PointAtInfinity;
    Point n = point;
    
    // Find first non-zero bit starting at the MSB of the scalar.
    int i = static_cast<int>(scalar.GetMostSignificantBitIndex());
    
    // Do the addition based on the above algorithm.
    for(; i >= 0; i--)
    {
        product = AddPointsOnCurve(product, product);
        if(scalar.GetBitAt(i))
            product = AddPointsOnCurve(product, point);
    }
    
    return product;
}

bool EllipticCurve::CheckPointOnCurve(const Point& point) const
{
    // For the point (x,y) to be on the curve, the x and y coordinates must satisfy the curve equation:
    //  y^2 = x^3 + ax + b    mod p
    
    FieldElement leftHandSide = point.y * point.y;
    FieldElement rightHandSide = (point.x * point.x * point.x) + (_a * point.x) + _b;
    bool pointIsOnCurve =  rightHandSide == leftHandSide;
    
    return pointIsOnCurve;
}

Point EllipticCurve::MakePointOnCurve(BigInteger&& x, BigInteger&& y) const
{
    Point point(FieldElement(move(x), _p), FieldElement(move(y), _p));
    
    // Test to ensure that the point is on the curve.
    if(!CheckPointOnCurve(point))
        throw invalid_argument("Point not on curve.");
    
    return point;
}

Point EllipticCurve::MakePointOnCurve(const vector<uint8_t>& serializedPoint) const
{
    Point point = Point::Parse(serializedPoint, 0, _p);
    
    // Test to ensure that the point is on the curve.
    if(!CheckPointOnCurve(point))
        throw invalid_argument("Point not on curve.");
    
    return point;
}

const Point& EllipticCurve::GetBasePoint() const
{
    return _G;
}

const BigInteger& EllipticCurve::GetBasePointOrder() const
{
    return _n;
}

string EllipticCurve::GetCurveName() const
{
    return _curveName;
}