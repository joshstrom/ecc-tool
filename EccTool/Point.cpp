//
//  Point.cpp
//  EccTool
//
//  Created by Josh Strom on 2/2/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "Point.h"
#include <stdio.h>

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os << "{" << point.x << "," << point.y << "}";
    return os;
}