//
//  Point.cpp
//  EccTool
//
//  Created by Josh Strom on 2/2/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "Point.h"
#include <stdio.h>

const char* Point::COMPRESSED_POINT_FLAG = "02";
const char* Point::UNCOMPRESSED_POINT_FLAG = "04";

// Creates a point at infinity.
Point Point::MakePointAtInfinity()
{
    return Point(true);
}

Point Point::Parse(const string& serializedPoint, shared_ptr<BigInteger> field)
{
    // Strip out any spaces in the generator.
    string point(serializedPoint);
    point.erase(std::remove_if(point.begin(), point.end(), ::isspace), point.end());
    
    if(point.size() < 2)
        throw invalid_argument("Valid generator not provided: buffer too short.");
    
    // Pull the first two characters from the beginning of the string, this indicates whether the point is compressed
    //  or uncompressed.
    string type = point.substr(0, 2);
    if(type == COMPRESSED_POINT_FLAG)
        return ParseCompressedGeneratorPoint(point.substr(2, string::npos), move(field));
    if(type == UNCOMPRESSED_POINT_FLAG)
        return ParseUncompressedGeneratorPoint(point.substr(2, string::npos), move(field));
    
    throw invalid_argument("Invalid point compression flag.");
}

Point Point::ParseUncompressedGeneratorPoint(const string& pointString, shared_ptr<BigInteger> field)
{
    // The first half of the point is the x-coordinate, the second half of the point is the y-coordinate.
    if((pointString.size() % 2) != 0)
        throw invalid_argument("Point not serialized correct: x not same size as y.");
    
    // Create the point: x = first half of point string, y = second half of point string.
    BigInteger pointX(string(pointString.begin(), pointString.begin() + (pointString.size() / 2)));
    BigInteger pointY(string(pointString.begin() + (pointString.size() / 2), pointString.end()));
    
    return Point(FieldElement(move(pointX), field), FieldElement(move(pointY), field));
}

Point Point::ParseCompressedGeneratorPoint(const string& pointString, shared_ptr<BigInteger> field)
{
    throw invalid_argument("compressed generator point parsing not implemented.");
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

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os << "{" << point.x << "," << point.y << "}";
    return os;
}