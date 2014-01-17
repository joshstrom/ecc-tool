//
//  BigInteger.cpp
//  EccTool
//
//  Created by Josh Strom on 1/14/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "BigInteger.h"

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

BigInteger::BigInteger(string number)
{
    for(char digit : number)
    {
        unsigned char current = static_cast<unsigned char>(tolower(digit) - '0');
        _source.push_back(current);
    }
}

const string BigInteger::ToString() const
{
    stringstream ss;
    ss << hex;
    
    for(auto digit : _source)
        ss << setw(2) << setfill('0') << static_cast<unsigned short>(digit);
    return ss.str();
}