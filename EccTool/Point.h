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
    static Point Parse(const vector<uint8_t>& serializedPoint, size_t offset, shared_ptr<BigInteger> field);
    
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
    
    // Gets the size of this point when serialized uncompressed.
    size_t ComputeUncompressedSize() const;
    
    static size_t ComputeUncompressedPointSize(size_t fieldSize);
 
private:
    static const char* COMPRESSED_POINT_FLAG_STR;
    static const char* UNCOMPRESSED_POINT_FLAG_STR;
    
    static const char COMPRESSED_POINT_FLAG;
    static const char UNCOMPRESSED_POINT_FLAG;
    
    // Internal point parsing helper function for uncompressed point representations.
    static Point ParseUncompressedPoint(const vector<uint8_t>& serializedPoint, size_t offset, shared_ptr<BigInteger> field);

    // Internal point parsing helper function for compressed point representations.
    static Point ParseCompressedPoint(const vector<uint8_t>& serializedPoint, size_t offset, shared_ptr<BigInteger> field);
    
    // Determines if this is a point at infinity.
    bool isPointAtInfinity;

    // Internal constructor used only for making a point at infinity (bool param provided
    //  so these are not accidentally created as this is a default constructor).
    Point(bool isPointAtInfinity);
};

// Stream writing operator for Point.
std::ostream& operator<<(std::ostream& os, const Point& point);

#endif /* defined(__EccTool__Point__) */
