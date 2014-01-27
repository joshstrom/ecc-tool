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
    
    DomainParameters params({
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    });
    
    return 0;
}

