//
//  FieldElement.h
//  EccTool
//
//  Created by Josh Strom on 2/2/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__FieldElement__
#define __EccTool__FieldElement__

#include <iostream>
#include <memory>

#include "BigInteger.h"

using namespace std;

// FieldElement represents a number in a finite modular field and defines
//  the arithmetic operations on these elements.
class FieldElement
{
private:
    BigInteger _number;
    shared_ptr<const BigInteger> _p;
    
public:
    FieldElement(BigInteger number, shared_ptr<const BigInteger> p);
    FieldElement(BigInteger number, const BigInteger p);
    
    // Mathematical operations mod _p.
    // Definitions of the below modulo operations were found here: http://tools.ietf.org/search/rfc6090
    FieldElement& operator+=(const FieldElement& other);
    FieldElement& operator-=(const FieldElement& other);
    FieldElement& operator*=(const FieldElement& other);
    FieldElement& operator/=(const FieldElement& other);
    
    // Returns additive inverse.
    FieldElement operator-() const;
    
    // Functions to find the multiplicative inverse of this element.
    FieldElement& Invert();
    FieldElement GetInverse() const;
    
    // Comparison Operators
    bool operator==(const FieldElement& other) const;
    bool operator!=(const FieldElement& other) const;
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;
    
    BigInteger GetRawInteger() const;
    string ToString() const;
};

// Binary '+' operator implemented as a free function by convention.
FieldElement operator+(FieldElement lhs, const FieldElement& rhs);

// Binary '-' operator implemented as a free function by convention.
FieldElement operator-(FieldElement lhs, const FieldElement& rhs);

// Binary '*' operator implemented as a free function by convention.
FieldElement operator*(FieldElement lhs, const FieldElement& rhs);

// Binary '/' operator implemented as a free function by convention.
FieldElement operator/(FieldElement lhs, const FieldElement& rhs);

// Streaming operator used for printing object to stream.
std::ostream& operator<<(std::ostream& os, const FieldElement& point);

#endif /* defined(__EccTool__FieldElement__) */
