//
//  EccDefs.h
//  EccTool
//
//  Created by Josh Strom on 1/25/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_EccDefs_h
#define EccTool_EccDefs_h

#include <string>
#include <ostream>

using namespace std;

namespace ecc
{
    // Forward declarations.
    struct Point;
    struct Parameters;
    
    // Defines the x and y coordinates of a point on the curve.
    struct Point
    {
        // The x-coordinate of the point.
        BigInteger x;
        // The y-coordiante of the point.
        BigInteger y;
        
        // Determines if this is a point at infinity.
        const bool isPointAtInfinity;
        
        // Two points are equal if:
        //  a) They are both a point at infinity (x,y coordinates don't matter).
        //  b) They have the exact same x and y coordinates.
        bool operator==(const Point& other) const
        {
            return (isPointAtInfinity && other.isPointAtInfinity) || ((x == other.x) && (y == other.y));
        }
        
        // Not Equal implemented as the inverse of Equal.
        bool operator!=(const Point& other) const
        {
            return !(*this == other);
        }
        
        // Creates a point with the given x and y coordinates (not at infinity).
        Point(BigInteger x, BigInteger y) : x(x), y(y), isPointAtInfinity(false)
        {
        }
        
        // Creates a point at infinity.
        static ecc::Point MakePointAtInfinity()
        {
            return Point(true);
        }
        
    private:
        // Internal constructor used only for making a point at infinity (bool param provided
        //  so these are not accidentally created as this is a default constructor).
        Point(bool isPointAtInfinity) : x(0), y(0), isPointAtInfinity(isPointAtInfinity)
        {
        }
    };
    
    // Stream writing operator for Point.
    inline std::ostream& operator<<(std::ostream& os, const Point& point)
    {
        os << "{" << point.x << "," << point.y << "}";
        return os;
    }

    // Defines the parameters that define the elliptic curve equation: y^2 = x^3 + ax + b mod p.
    struct DomainParameters
    {
        // The prime number which defines the field (Fp) in which the curve operates.
        const string p;
        
        // The two coefficients which define the curve:
        const string a;
        const string b;
        
        // The Generator or Base Point
        const string G;
        
        // The order of the curve generator point G.
        const string n;
        
        // The cofactor of the curve.
        const string h;
    };
    
    
}

#endif
