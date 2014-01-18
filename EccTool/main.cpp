//
//  main.cpp
//  EccTool
//
//  Created by Josh Strom on 1/9/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include <iostream>

#include "EccAlg.h"
#include "BigInteger.h"

using namespace std;

int main(int argc, const char * argv[])
{
    BigInteger left("12345");

    BigInteger sum = left + static_cast<unsigned int>(1);

    // insert code here...
    std::cout << "Left: " << left.ToString() << ", Right: " << 1 << ", Sum: " << sum.ToString() << endl;
    return 0;
}

