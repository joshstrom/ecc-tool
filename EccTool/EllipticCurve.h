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
#ifndef __EccTool__EllipticCurve__
#define __EccTool__EllipticCurve__

#include <iostream>
#include <string>
#include <memory>
#include "BigInteger.h"
#include "EccDefs.h"
#include "Point.h"

using namespace std;
using namespace ecc;

//BigInteger ModularMultiply(BigInteger& rhs)

class EllipticCurve
{
    // --
    // Members/functions of EllipticCurve
    // --
private:
    static const char* COMPRESSED_GENERATOR_FLAG;
    static const char* UNCOMPRESSED_GENERATOR_FLAG;
    
    // The parameters for this particular curve.
    // The field Fp over which the equation operates.
    shared_ptr<BigInteger> _p;
    
    // The coefficients which define the curve.
    FieldElement _a;
    FieldElement _b;

    // The Generator or Base Point of this curve.
    Point _G;
    
    // The order of the curve generator point G.
    BigInteger _n;
    
    // The cofactor of the curve.
    BigInteger _h;
    
    // Name of the curve.
    string _curveName;
    
    // Internal functions to compute the addition of two points for
    //  a) A + B = C (when A and B are distinct), and
    //  b) A + A = C (when A is added to itself)
    Point PointAdd(const Point& rhs, const Point& lhs) const;
    Point PointDouble(const Point& point) const;
    
public:
    // Point at infinity.
    static const Point PointAtInfinity;
    
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
    
    // Creates a point on the curve from x and y coordinates.
    Point MakePointOnCurve(BigInteger&& x, BigInteger&& y) const;
    
    // Creates a point on the curve from a serialized point.
    Point MakePointOnCurve(const vector<uint8_t>& serializedPoint) const;
    
    // Returns the Generator G, or Base Point of this curve.
    const Point& GetBasePoint() const;
    
    // Returns the order of the Generator G.
    const BigInteger& GetBasePointOrder() const;
    
    // Gets the name of this particular curve.
    string GetCurveName() const;
};

#endif /* defined(__EccTool__EllipticCurve__) */
