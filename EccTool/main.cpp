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
    BigInteger left(0xffffffff);
    BigInteger right(0xffffffff);

    BigInteger product = left * right;
    string expected("fffffffe00000001");

    // insert code here...
    std::cout << "Left: " << left.ToString() << ", Right: " << right.ToString() << ", Product: " << product.ToString() << ", expected: " << expected << endl;
    return 0;
}

