//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Joshua Strom
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
#include "FieldElement.h"
#include <sstream>
#include <cassert>

FieldElement FieldElement::MakeElement(BigInteger number, shared_ptr<const BigInteger> p)
{
    assert(number >= 0);
    if(number >= *p)
        number %= *p;
    
    return FieldElement(move(number), p);
}

FieldElement FieldElement::MakeElement(const FieldElement& fieldNumber, shared_ptr<const BigInteger> p)
{
    BigInteger number = fieldNumber.GetRawInteger();
    return MakeElement(move(number), p);
}

FieldElement::FieldElement(BigInteger number, shared_ptr<const BigInteger> p)
    : _number(number), _p(p)
{
    // Number must be within the finite field. This test is done as a debug
    //  assert since numbers are not selected by users and the issue will appear
    //  with any code issues.
    assert(_number >= 0 && _number < *_p);
}

FieldElement::FieldElement(BigInteger number, BigInteger p)
: _number(number), _p(make_shared<const BigInteger>(p))
{
    // Number must be within the finite field. This test is done as a debug
    //  assert since numbers are not selected by users and the issue will appear
    //  with any code issues.
    assert(_number >= 0 && _number < *_p);
}

FieldElement& FieldElement::operator+=(const FieldElement& other)
{
    // Let p define the max of the finite field Fp such that all elemnets of Fp are in the range
    //  [0, p-1].
    // Since both operands are in Fp, the result of an addition operation must be
    //  in the range [0, 2p-1]. Thus, to ensure that the result of the addition is an element of Fp,
    //  do the following transformation:
    //      case (sum < p): result is sum
    //      case (sum > p): result is sum - p (which will place it back in the range [0, p-1]).
    _number += other._number;
    if(_number >= *_p)
        _number -= *_p;
    
    return *this;
}

FieldElement& FieldElement::operator-=(const FieldElement& other)
{
    // Let p define the max of the finite field Fp such that all elements of Fp are in the range
    //  [0, p-1].
    // Since both operands are in Fp, the result of a subtraction operation must be
    //  in the range [-(p-1), p-1]. Thus, to ensure that the result of the subtraction is an element of Fp,
    //  do the following transformation:
    //      case (result >= 0): return result.
    //      case (result < p): return result + p (which will place it back in the range [0, p-1].
    _number -= other._number;
    if(_number < 0)
        _number += *_p;
    
    return *this;
}

FieldElement& FieldElement::operator*=(const FieldElement& other)
{
    // Let p define the max of the finite field Fp such that all elements of Fp are in the range
    //  [0, p-1].
    // To ensure that the result of the multiplication operation is an element of Fp, take the quotient
    //  mod p, which will place the result in the range [0, p-1]
    _number *= other._number;
    _number %= *_p;
    
    return *this;
}

FieldElement& FieldElement::operator/=(const FieldElement& other)
{
    *this *= other.GetInverse();
    return *this;
}

FieldElement FieldElement::GetInverse() const
{
    FieldElement copy = *this;
    copy.Invert();
    
    return copy;
}

FieldElement& FieldElement::Invert()
{
    // Use the following algorithm to calculate the multiplicative inverse via the
    //  Extended Euclidean Algorithm: (algorithm found here: http://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)
    //    function eea(a, b)
    //        s := 0;    old_s := 1
    //        r := b;    old_r := a
	//		  t := 1;    old_t := 0 <- Not used.
    //        while r ≠ 0
    //            quotient := old_r div r
    //            (old_r, r) := (r, old_r - quotient *r)
    //            (old_s, s) := (s, old_s - quotient *s)
	//			  (old_t, t) := (t, old_t - quotient *t) <- Not done.
    //        output "Bézout coefficients:", (old_s, old_t)
    //        output "greatest common divisor:", old_r
    //        output "quotients by the gcd:", (t, s)
    
    //    Where (old_r, r) := (r, old_r - quotient *r) means:
    //        prov := r;
    //        r := old_r - quotient * prov;
    //        old_r := prov;
	// Note: this algorithm has been adjusted from that above for efficiency (through profiling)
	//	to avoid extra operations, extra copies and extra allocations. Additionally, we do not need t, so it
    //  is not computed.
    
    // This algorithm allows us to find a number which is the inverse of this FieldElement within
    // the finite field.
    
	// This will eventually hold the inverse of a mod b.
    BigInteger s = 0;
    BigInteger old_s = 1;
    
	// Used to hold the repeatedly computed remainder.
    BigInteger r = *_p;
    BigInteger old_r = _number;
    
    while(r != 0)
    {
		// Do the full division operation for the quotient and remainder.
        auto divResult = BigInteger::Divide(old_r, r); // Returns a pair: <quotient, remainder>
		BigInteger& quotient = divResult.first;
		BigInteger& remainder = divResult.second;
        
		swap(old_r, r); // Save away the current r in old_r (value of old_r stored in r and no longer needed)
		swap(r, remainder);	// Save away the current remainder in r (use swap to avoid making a copy).
        
		// Do the first part of the calculation (s = old_s - (quotient * s))
		//	(the multiplication part), reusing the BigInteger instance for quotient.
		quotient *= s;
        
		// Store away the value of s into old_s for the next iteration.
		swap(old_s, s);
        
		// Now that old_s is in s, do the subtraction part of the equation (s = old_s - (quotient * s).
		//	by subtracting the quotion (which holds quotient*s) from s (which holds old_s).
		s -= quotient;
    }
    
    // old_s is the multiplicative inverse of a, but may be a negative number.
    //  Place this within Fp by adding p to the result (if the result is less than 0).
    if(old_s < 0)
        old_s += *_p;
    
    swap(_number, old_s);
    return *this;
}

FieldElement FieldElement::operator-() const
{
    FieldElement result(0, _p);
    result -= *this;
    
    return result;
}

// ***
// comparison Operators
// ***
bool FieldElement::operator==(const FieldElement& other) const
{
    return (_number == other._number);
}

bool FieldElement::operator!=(const FieldElement& other) const
{
    return !(*this == other);
}

bool FieldElement::operator==(const BigInteger& other) const
{
    return (_number == other);
}

bool FieldElement::operator!=(const BigInteger& other) const
{
    return !(*this == other);
}

const BigInteger& FieldElement::GetRawInteger() const
{
    return _number;
}

string FieldElement::ToString() const
{
    stringstream ss;
    ss << '(' << _number << " mod " << *_p << ')';
    
    return ss.str();
}

vector<uint8_t> FieldElement::GetBytes() const
{
    auto bytes = _number.GetMagnitudeBytes();
    
    // Reize the vector to be returned by inserting the number of zero
    //  bytes required to size the number to be the same as the field.
    bytes.insert(bytes.begin(), _p->GetMagnitudeByteSize() - bytes.size(), 0);
    
    return bytes;
}

size_t FieldElement::GetByteSize() const
{
    return _p->GetMagnitudeByteSize();
}

// ***
// Implementatiions of free functions for binary mathematical operations
// ***
FieldElement operator+(FieldElement lhs, const FieldElement& rhs)
{
    lhs += rhs;
    return lhs;
}

FieldElement operator-(FieldElement lhs, const FieldElement& rhs)
{
    lhs -= rhs;
    return lhs;
}

FieldElement operator*(FieldElement lhs, const FieldElement& rhs)
{
    lhs *= rhs;
    return lhs;
}

FieldElement operator/(FieldElement lhs, const FieldElement& rhs)
{
    lhs /= rhs;
    return lhs;
}

std::ostream& operator<<(std::ostream& os, const FieldElement& point)
{
    os << point.ToString();
    return os;
}


