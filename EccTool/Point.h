//
//  Point.h
//  EccTool
//
//  Created by Josh Strom on 1/26/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__Point__
#define __EccTool__Point__

#include <iostream>
#include "BigInteger.h"
#include "FieldElement.h"

using namespace std;

// Defines the x and y coordinates of a point on the curve.
class Point
{
public:
    // The x-coordinate of the point.
    FieldElement x;
    // The y-coordiante of the point.
    FieldElement y;
    
    // Creates a point at infinity.
    static Point MakePointAtInfinity()
    {
        return Point(true);
    }
    
    Point() : x(BigInteger(0), BigInteger(0)), y(BigInteger(0), BigInteger(0)), isPointAtInfinity(false)
    {
    }
    
    // Creates a point with the given x and y coordinates (not at infinity).
    Point(FieldElement x, FieldElement y) : x(x), y(y), isPointAtInfinity(false)
    {
    }
    
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
 
private:
    // Determines if this is a point at infinity.
    bool isPointAtInfinity;

    // Internal constructor used only for making a point at infinity (bool param provided
    //  so these are not accidentally created as this is a default constructor).
    Point(bool isPointAtInfinity) : x(BigInteger(0), BigInteger(0)), y(BigInteger(0), BigInteger(0)), isPointAtInfinity(isPointAtInfinity)
    {
    }
};

// Stream writing operator for Point.
std::ostream& operator<<(std::ostream& os, const Point& point);

#endif /* defined(__EccTool__Point__) */
