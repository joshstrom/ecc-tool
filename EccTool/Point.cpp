//
//  Point.cpp
//  EccTool
//
//  Created by Josh Strom on 2/2/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "Point.h"
#include <stdio.h>
#include <iomanip>
#include <sstream>

const char* Point::COMPRESSED_POINT_FLAG_STR = "02";
const char* Point::UNCOMPRESSED_POINT_FLAG_STR = "04";

const char Point::COMPRESSED_POINT_FLAG = 2;
const char Point::UNCOMPRESSED_POINT_FLAG = 4;

// Creates a point at infinity.
Point Point::MakePointAtInfinity()
{
    return Point(true);
}

Point Point::ParseUncompressedPoint(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field)
{
    // Format of uncompressed point: <compression flag><x-coordinate><y-coordinate> with the
    //  compression flag being a single byte and the x/y coordinates represented in the same number of bytes.
    //
    // Thus the length of the buffer can be
    //  calculated with the expression 2n + 1 (where n is the number of bytes to represent the x or the
    //  y coordinate). Therefore, the buffer must be of an odd number of bytes (by definition of odd).
    //  Fail if this is not the case.
    if(serializedPoint.size() % 2 == 0)
        throw invalid_argument("Invalid length of point buffer");
    
    // Since the length of the buffer is equal to 2n + 1, to discover n, we must solve the expression
    //  n = (<length of buffer> - 1)/2. Observe that <length of buffer> - 1 must be even, so the division
    //  will result in an integer.
    auto sizeOfCoordinates = (serializedPoint.size() - 1)/2;
    
    // The x and y coordinate segments can be found by adjusting the beginning and end iterators of the
    //  serialized point buffer to skip over the point compression flag and/or the other coordinate.
    BigInteger xCoord(vector<uint8_t>(serializedPoint.begin() + 1, serializedPoint.end() - sizeOfCoordinates));
    BigInteger yCoord(vector<uint8_t>(serializedPoint.begin() + 1 + sizeOfCoordinates, serializedPoint.end()));
    
    return Point(FieldElement(move(xCoord), field), FieldElement(move(yCoord), field));
}

Point Point::Parse(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field)
{
    // Format of serialized point <compression flag><serialized point>, parsing is different
    //  depending on compression flag.
    if(serializedPoint.size() < 1) // Net even a compression flag.
     throw invalid_argument("Buffer too small to hold point");
    
    uint8_t compressionFlag = serializedPoint[0];
    switch (compressionFlag) {
        case UNCOMPRESSED_POINT_FLAG:
            return ParseUncompressedPoint(serializedPoint, field);
        case COMPRESSED_POINT_FLAG:
            return ParseCompressedPoint(serializedPoint, field);
        default:
            throw invalid_argument("Invalid point compression flag.");
    }
}

Point Point::ParseCompressedPoint(const vector<uint8_t>& serializedPoint, shared_ptr<BigInteger> field)
{
    // TODO: Implement.
    throw invalid_argument("Compressed point parsing not implemented.");
}

Point::Point() : x(BigInteger(0), BigInteger(1)), y(BigInteger(0), BigInteger(1)), isPointAtInfinity(false)
{
}

Point::Point(FieldElement x, FieldElement y) : x(x), y(y), isPointAtInfinity(false)
{
}

Point::Point(bool isPointAtInfinity) : x(BigInteger(0), BigInteger(1)), y(BigInteger(0), BigInteger(1)), isPointAtInfinity(isPointAtInfinity)
{
}

bool Point::operator==(const Point& other) const
{
    return (isPointAtInfinity && other.isPointAtInfinity)
    || ((!isPointAtInfinity && !other.isPointAtInfinity) && ((x == other.x) && (y == other.y)));
}

bool Point::operator!=(const Point& other) const
{
    return !(*this == other);
}

// Serialzes the point to a binary representation.
vector<uint8_t> Point::Serialize() const
{
    // The point is written in an uncompressed format: 04<x coordinate><y coordinate>.
    auto xCoord = x.GetBytes();
    auto yCoord = y.GetBytes();

    vector<uint8_t> result(1 + xCoord.size() + yCoord.size());
    auto iterator = result.begin();
    *iterator = UNCOMPRESSED_POINT_FLAG;
    iterator++;
    
    copy(xCoord.begin(), xCoord.end(), iterator);
    iterator += xCoord.size();
    
    copy(yCoord.begin(), yCoord.end(), iterator);
    
    return result;
}

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os << "{" << point.x << "," << point.y << "}";
    return os;
}
















