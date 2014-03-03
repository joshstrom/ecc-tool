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
    // Creates a field element from a big integer and a field p. If the number is not
    // already within p, the number is taken modulo p. Number must be >= 0.
    static FieldElement MakeElement(BigInteger number, shared_ptr<const BigInteger> p);
    
    // Creates a field element from a big integer and a field p. If the number is not
    // already within p, the number is taken modulo p. Number must be >= 0.
    static FieldElement MakeElement(const FieldElement& number, shared_ptr<const BigInteger> p);
    
    // Constructor taking an number in the field and the field itself (as a shared ptr).
    FieldElement(BigInteger number, shared_ptr<const BigInteger> p);
    
    // Constructor taking an number in the field and the field itself (by value).
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
    
    // Comparison Operators specialized for other FieldElements and BigIntegers.
    bool operator==(const FieldElement& other) const;
    bool operator!=(const FieldElement& other) const;
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;
    
    // Returns the element as a BigInteger.
    const BigInteger& GetRawInteger() const;
    
    // Gets a string representation of this field element (mod n).
    string ToString() const;
    
    // Returns this field element as bytes. The array is sized to be
    //  the same as the field, and prepended with zeros.
    vector<uint8_t> GetBytes() const;
    
    // Returns the number of bytes it would take to serialize the integer
    // same size as the field.
    size_t GetByteSize() const;
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
ostream& operator<<(ostream& os, const FieldElement& point);

#endif /* defined(__EccTool__FieldElement__) */
