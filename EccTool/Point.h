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
#include "Utilities.h"

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
    static Point MakePointAtInfinity();
    
    // Deserializes a point.
    static Point Parse(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field);
    
    // Default constructor, creates a point at (0,0).
    Point();
    
    // Creates a point with the given x and y coordinates (not at infinity).
    Point(FieldElement x, FieldElement y);
    
    // Two points are equal if:
    //  a) They are both a point at infinity (x,y coordinates don't matter).
    //      OR
    //  b) Neither is a point at infinity and they have the exact same x and y coordinates.
    bool operator==(const Point& other) const;
    
    // Not Equal implemented as the inverse of Equal.
    bool operator!=(const Point& other) const;
    
    // Serialzes the point to a string representation.
    vector<uint8_t> Serialize() const;
 
private:
    static const char* COMPRESSED_POINT_FLAG_STR;
    static const char* UNCOMPRESSED_POINT_FLAG_STR;
    
    static const char COMPRESSED_POINT_FLAG;
    static const char UNCOMPRESSED_POINT_FLAG;
    
    // Internal point parsing helper function for uncompressed point representations.
    static Point ParseUncompressedPoint(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field);

    // Internal point parsing helper function for compressed point representations.
    static Point ParseCompressedPoint(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field);
    
    // Determines if this is a point at infinity.
    bool isPointAtInfinity;

    // Internal constructor used only for making a point at infinity (bool param provided
    //  so these are not accidentally created as this is a default constructor).
    Point(bool isPointAtInfinity);
};

// Stream writing operator for Point.
std::ostream& operator<<(std::ostream& os, const Point& point);

#endif /* defined(__EccTool__Point__) */
