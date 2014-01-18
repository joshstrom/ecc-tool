//
//  BigInteger.h
//  EccTool
//
//  Created by Josh Strom on 1/14/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__BigInteger__
#define __EccTool__BigInteger__

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>

using namespace std;

class BigInteger
{
private:
    vector<uint8_t> _source;
    
    uint8_t GetValidHexDigit(char digit) const;
    
public:
    BigInteger(string number);
    
    const string ToString() const;
    
};

#endif /* defined(__EccTool__BigInteger__) */
