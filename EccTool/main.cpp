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
    BigInteger left("10h");
    BigInteger right("ah");

    BigInteger sum = left + right;

    // insert code here...
    std::cout << "Left: " << left.ToString() << ", Right: " << right.ToString() << ", Sum: " << sum.ToString() << endl;
    return 0;
}

