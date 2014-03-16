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
#include <math.h>
#include <exception>
#include <sstream>
#include <algorithm>
#include "Utilities.h"

using namespace std;

BigInteger::BigInteger(string number)
{
    // Determine sign of number.
    _sign = ((!number.empty() && number[0] == '-') ? NEGATIVE : POSITIVE);
    
    // Negative number means that the string was not empty the first character was '-'
    //  Strip off the negative sign.
    if(_sign == NEGATIVE)
        number = number.substr(1, string::npos);
        
    _magnitude = utilities::HexStringToBytes(move(number));
    
    // Remove any empty zero bytes from the beginning of the buffer.
    TrimPrefixZeros();
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs)
{
    Sign thisSign = GetSign();
    Sign rhsSign = rhs.GetSign();
    
	// If the signs were the same, add magnitudes as normal and do not change the sign.
	if(thisSign == rhsSign)
    {
        Add(rhs);
        return *this;
    }
    
	// If one is negative and the other is positive, adding them together will
	//	result in a number that has a magnitude of the larger minus the smaller
	//	and a sign of the largest one.
	int magnitudeComparisonResult = CompareMagnitudeTo(rhs);
	if(magnitudeComparisonResult == 0) // Magnitude equal -> result: 0
	{
		SetZero();
	}
	else if(magnitudeComparisonResult < 0) // This is smaller than that: result -> (that-this) with sign of that.
	{
		BigInteger subtractionResult = rhs; // Swap and temporary needed for now. TODO: fix?
		subtractionResult.Subtract(*this); 
		_magnitude.swap(subtractionResult._magnitude);
		_sign = rhs.GetSign();
	}
	else // This is larger than that: result -> (this-that) with sign of this (no change to sign).
	{
		Subtract(rhs);
	}
	
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs)
{
	// Implement subtraction in terms of addition... add the inverse.
	*this += -rhs;
	return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs)
{
	// Check for multiplication by zero.
	if(IsZero() || rhs.IsZero())
	{
		SetZero();
		return *this;
	}

	// Do the multiplication. 
    Multiply(rhs);

	// The result sign is positive if both signs are the same.
	// The result is negative if they are different.
	_sign = (GetSign() == rhs.GetSign()) ? POSITIVE : NEGATIVE;

    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& divisor)
{
    // Do the actual operation.
    auto quotient = Divide(*this, divisor).first;
    
    // Since the divide helper function only deals in magnitude and does
    //  not handle sign, determine the sign of the result by the comparing the
    //  signs of the operands.
    Sign newSign = (GetSign() == divisor.GetSign()) ? POSITIVE : NEGATIVE;
    
    // Swap the quotient result in the this instance and set the calcualted sign.
    swap(*this, quotient);
    _sign = newSign;
    
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& divisor)
{
    // Simplification in the case of a positive divisor
    if(divisor.GetSign() == POSITIVE)
    {
        // If the numerator (this) is non-negative and less than the divisor,
        //  the operation is a no-op.
        if((GetSign() == POSITIVE) && (CompareMagnitudeTo(divisor) < 0))
            return *this;
    }
    
    // Do the actual operation.
    auto remainder = Divide(*this, divisor).second;
    
    // Place the remainder of the division operation in this instance.
    swap(*this, remainder);
    return *this;
}

// Prefix-increment.
//  Applies the change to the return value.
BigInteger& BigInteger::operator++()
{
    *this += 1;
    return *this;
}

//Postfix-increment.
//  Applies the change *after* returning the same value. Change "this", return a copy.
//  Note: the int param is a dummy param used to determine the difference between the prefix
//  and postfix versions.
BigInteger BigInteger::operator++(int)
{
    BigInteger copy = *this;
    ++*this;
    
    return copy;
}

// Prefix-decrement.
//  Applies the change to the return value.
BigInteger& BigInteger::operator--()
{
    *this -= 1;
    return *this;
}

//Postfix-decrement.
//  Applies the change *after* returning the same value. Change "this", return a copy.
//  Note: the int param is a dummy param used to determine the difference between the prefix
//  and postfix versions.
BigInteger BigInteger::operator--(int)
{
    BigInteger copy = *this;
    --*this;
    
    return copy;
}

BigInteger BigInteger::operator-() const
{
    BigInteger negated = *this;
    
    if(negated.IsZero())
        return negated;
    
    if(negated.GetSign() == POSITIVE)
    {
        negated._sign = NEGATIVE;
        return negated;
    }
    
    negated._sign = POSITIVE;
    return negated;
}

bool BigInteger::operator<(const BigInteger& rhs) const
{
    return (CompareTo(rhs) == -1);
}

bool BigInteger::operator>(const BigInteger& rhs) const
{
    return (CompareTo(rhs) == 1);
}

bool BigInteger::operator<=(const BigInteger& rhs) const
{
    return (CompareTo(rhs) <= 0);
}

bool BigInteger::operator>=(const BigInteger& rhs) const
{
    return (CompareTo(rhs) >= 0);
}

bool BigInteger::operator==(const BigInteger& other) const
{
    return (CompareTo(other) == 0);
}

bool BigInteger::operator!=(const BigInteger& other) const
{
    return !(*this == other);
}

// Bitwise indexing operators.
bool BigInteger::GetBitAt(size_t index) const
{
    uint8_t bitOffset = index % 8;
    uint8_t bitMask = 1 << bitOffset;
    
    return (_magnitude[(_magnitude.size() - 1) - (index / 8)] & bitMask) == bitMask;
}

void BigInteger::SetBitAt(size_t index)
{
    // Compute a bitmast for the bit portion of of the index.
    uint8_t bitOffset = index % 8;
    uint8_t bitMask = 1 << bitOffset;
    
    // A binary OR with that bitmask will set the bit.
    _magnitude[(_magnitude.size() - 1) - (index / 8)] |= bitMask;
}

void BigInteger::ClearBitAt(size_t index)
{
    // Compute a bitmast for the bit portion of of the index.
    uint8_t bitOffset = index % 8;
    uint8_t bitMask = 1 << bitOffset;
    
    // A binary AND with the inverse of that bitmask will clear the indicated bit.
    _magnitude[(_magnitude.size() - 1) - (index / 8)] &= ~bitMask;
    
    // This may have cleared the the last non-zero bit in the most significant byte.
    //  Trim if necessary.
    TrimPrefixZeros();
}

size_t BigInteger::GetBitSize() const
{
    int bitSize =  static_cast<int>(_magnitude.size()) * 8;
    for(int i = bitSize - 1; i >= 0; --i)
    {
        if(GetBitAt(i))
            return i + 1;
    }
    
    return 0;
}

size_t BigInteger::GetMostSignificantBitIndex() const
{
    return GetBitSize() - 1;
}

BigInteger& BigInteger::operator<<=(int count)
{
    // Zero shifted left is still zero.
    if(IsZero())
        return *this;
    
    // Determine the number of whole bytes and the number of bits to shift.
    int bytesToShift = count / 8;
    int bitsToShift = count % 8;
    
    if(bytesToShift != 0)
    {
        // Shift the magnitude vector by whole bytes.
        _magnitude.resize(_magnitude.size() + bytesToShift, 0);
    }
    
    if(bitsToShift == 0)
        return *this;
    
    // Shift each byte in the magnitude buffer, from right to left, by the number of bits to shift.
    //  Track any carry between iterations.
    uint8_t carry = 0;
    uint8_t remainingBitShiftAmount = 8 - bitsToShift;
    for(int i = static_cast<int>(_magnitude.size()) - 1; i >= 0; i--)
    {
        uint8_t& currentElement = _magnitude[i];
        
        // Determine the carry for the next byte (the most significant digits which would shift out).
        uint8_t nextCarry = (currentElement >> remainingBitShiftAmount);
        
        // Do the shift and add the previous carry.
        currentElement <<= bitsToShift;
        currentElement |= carry;
        
        // Save the current carry for the next iteration.
        carry = nextCarry;
    }
    
    // Handle any remaining carry.
    if(carry != 0)
        _magnitude.insert(_magnitude.begin(), carry);
    
    return *this;
}

BigInteger& BigInteger::operator>>=(int count)
{
    // Zero shifted right is still zero.
    if(IsZero())
        return *this;
    
    // Determine the number of whole bytes and the number of bits to shift.
    unsigned int bytesToShift = count / 8;
    unsigned int bitsToShift = count % 8;
    
    // If we are shifting by as many (or more) bytes than the magnitude, the result is zero.
    if(bytesToShift >= _magnitude.size())
    {
        SetZero();
        return *this;
    }
    
    // Delete the number of whole bytes to shift from the back of the vector.
    if(bytesToShift >= 0)
        _magnitude.resize(_magnitude.size() - bytesToShift);
    
    if(bitsToShift == 0)
        return *this;
    
    // Iterate through each byte in the array, from right to left, shifting each byte
    // to the right by the specifed number of bits, capturing the overflow, and applying
    // it to the next byte.
    
    // Construct a bit mask which will allow us to determine the carry for the next byte.
    // For example, if the nuber of bits to shift is 3, the mask should be 00000111b.
    // We can use bit shifting to accomplish this.
    uint8_t bitsToShiftInverse = 8 - bitsToShift;
    uint8_t carryMask = 0xff >> bitsToShiftInverse;
    
    uint8_t carry = 0;
    for(size_t i = 0; i < _magnitude.size(); i++)
    {
        uint8_t& current = _magnitude[i];
        
        // Determine the carry for the next byte by masking the byte with the
        // carry mask and shifting those bits to be the most significant.
        // Example: If the bit is: 10010100b, and we are shifting 3 bits,
        // the mask is: 0000111b, after the shift the carry is 11100000b.
        uint8_t nextCarry = (current & carryMask) << bitsToShiftInverse;
        
        // Shift the current bit and add the previous carry.
        current >>= bitsToShift;
        current |= carry;
        
        // Save the next carry.
        carry = nextCarry;
    }
    
    // Because of right shift, any overflow after the last byte is dropped.
    
    // We may have zero bytes at the beginning.
    TrimPrefixZeros();
    
    return *this;
}

const string BigInteger::ToString() const
{
    // Set up the stringstream for printing. We want output to be hex and
    //  the fill digit to be '0' (note that these settings are "sticky").
    stringstream ss;
    ss << hex << setfill('0');
    
    // Print negative (if necessary).
    if(_sign == NEGATIVE)
        ss << '-';
    
    // Print all digits to the stream.
    //  The width must be 2 (example: 0x5 should print as "05").
    //  The cast is to turn it from a character to a number.
    for(auto digit : _magnitude)
        ss << setw(2) << (static_cast<uint16_t>(digit) & 0xFF);
    return ss.str();
}

const vector<uint8_t>& BigInteger::GetMagnitudeBytes() const
{
    return _magnitude;
}

size_t BigInteger::GetMagnitudeByteSize() const
{
    return _magnitude.size();
}

bool BigInteger::IsZero() const
{
    return ((_magnitude.size() == 1) && (_magnitude[0] == 0));
}

BigInteger::Sign BigInteger::GetSign() const
{
    if(IsZero())
        return POSITIVE;
    
    return _sign;
}

void BigInteger::TrimPrefixZeros()
{
    // Remove any empty zero bytes from the beginning of the buffer.
	auto size = _magnitude.size();
	unsigned int firstNonzero = 0;
	for(; firstNonzero < size; ++firstNonzero)
		if(_magnitude[firstNonzero] != 0) break;

	auto begin = _magnitude.begin();
    _magnitude.erase(begin, begin + firstNonzero);
    
    // If this trimming results in an empty buffer, place a single zero.
    if(_magnitude.size() == 0)
        _magnitude.push_back(static_cast<uint8_t>(0));
}

void BigInteger::SetZero()
{
	_magnitude.resize(1);
	_magnitude[0] = 0;
	_sign = POSITIVE;
}

void BigInteger::Borrow(vector<uint8_t>& buffer, int segmentBegin) const
{
    // Search ahead for the first non-zero character in the buffer.
    //  For the borrow to work correctly, it must populate back across
    //  previous elements in the buffer.
    //  Example (in decimal): Borrow starting from the ones place of 1000 -> 099(10)
    while(segmentBegin >= 0)
    {
        // Correct element found. Decrement and return.
		uint8_t& currentElement = buffer[segmentBegin];
        if(currentElement > 0)
        {
            --currentElement;
            return;
        }
        
        // Element equal to zero, we need to adjust for the borrow.
        currentElement = 0xFF;
        --segmentBegin;
    }
}

// Returns -1, zero, 1 as this object is less than, equal to, or greater than the specified object.
int BigInteger::CompareTo(const BigInteger& other) const
{
    // If the signs are different, the positive one is larger than the negative one.
    const Sign thisSign = GetSign();
    const Sign otherSign = other.GetSign();
    if(thisSign != otherSign)
    {
        return (thisSign == POSITIVE) ? 1 : -1;
    }
    
    // If the signs are the same, compare magnitude and flip the result if the signs
    //  were both negative.
    int compareResult = CompareMagnitudeTo(other);
    return (thisSign == POSITIVE) ? compareResult : -compareResult;
}

// Returns -1, zero, 1 as the absolute value of this object is less than, equal to, or greater
//  than the absolute value of the specified object.
int BigInteger::CompareMagnitudeTo(const BigInteger& other) const
{
    // BigIntegers store sign separately from magnitude. Simply compare magnitude.
    
    // Since there are no zero bytes at the beginnig of the arrays,
    //  if one is longer than the other, it is larger than the other.
    if(_magnitude.size() < other._magnitude.size())
        return -1;
    else if(_magnitude.size() > other._magnitude.size())
        return 1;
    
    // Find the first element (from the left) which differs between this and other.
    // Use that element to determine which is greater.
    for(unsigned int i = 0; i < _magnitude.size(); i++)
    {
        if(_magnitude[i] == other._magnitude[i])
            continue;
        return (_magnitude[i] < other._magnitude[i]) ? -1 : 1;
    }
    
    // None were different. They are the same.
    return 0;

}

BigInteger& BigInteger::Add(const BigInteger& rhs)
{
    // The operation will be done as a long-hand addition.
    // Example:
    //     (topNumber)      12345   [the larger (or longer) of the two]
    //  (bottomNumber)    + 45678   [the smaller (or shorter) of the two] -- If both are the same length, we don't care.
    //                    -------
    //           (sum)      58023
    //
    // The "topNumber" and "bottomNumber" buffers will be acceessed with
    //  reverse iterators in order to add in the correct order.
    // A "carry" variable is also utilized.
    
    auto& topNumberBuffer = (_magnitude.size() > rhs._magnitude.size()) ? _magnitude : rhs._magnitude;
    auto& bottomNumberBuffer = (_magnitude.size() > rhs._magnitude.size()) ? rhs._magnitude : _magnitude;
    
    // Prepare the _magnitude buffer to hold the sum.
    //  Since an addition operation requires at most (longest number + 1) digits. Insert enough
    //  elements at the beginning to ensure that the buffer is large enough.
    _magnitude.insert(_magnitude.begin(), (topNumberBuffer.size() + 1 - _magnitude.size()), 0);

	int sumIndex = static_cast<int>(_magnitude.size()) - 1;
	int topNumberIndex = static_cast<int>(topNumberBuffer.size()) - 1;
	int bottomNumberIndex = static_cast<int>(bottomNumberBuffer.size()) - 1;

    uint8_t carry = 0;
    // Addition can result in a number that is (maximum) one extra character
    //  than the largest of the two operands. The "sum" buffer will be resized to this size.
    while(topNumberIndex >= 0)
    {
        // We will always have a top number (as we are looping over topNumberBuffer), however,
        //  the bottom number might have finished already. Use 0 if this is the case.
        uint8_t currentTop = topNumberBuffer[topNumberIndex];
        uint8_t currentButtom = (bottomNumberIndex >= 0) ? bottomNumberBuffer[bottomNumberIndex] : 0;
        
        // Do the addition, then take the top 4 bytes for the carry and push the bottom 4 bytes as the sum.
        uint16_t currentSum = currentTop + currentButtom + (carry >> 4);
        carry = (currentSum & 0xFF00) >> 4;
        _magnitude[sumIndex] = (currentSum & 0x00FF);
        
        // TopNumber and SumBuffer are both of the same size.
        --topNumberIndex;
		--sumIndex;
        
        // Increment past end is fine. We check before dereferencing.
        --bottomNumberIndex;
    }
    
    // If there is any leftover carry, add it to the sum buffer (shift it since it is one place up).
    if(carry != 0)
        _magnitude[sumIndex] = (carry >> 4);
    
    // The sum buffer may be too large (due to the insert operation at the beginning), remove extra.
    TrimPrefixZeros();
    
    return *this;
}

BigInteger& BigInteger::Multiply(const BigInteger& rhs)
{
    // The operation will be done as a long-hand multiplication.
    // Example:
    //     (topNumber)        123   [the larger of the two]
    //  (bottomNumber)    *    11   [the smaller of the two]
    //                    -------
    //      (products)        123   [multiply, digit-wise, each of the upper digits by the lower, ]
    //                    +  1230   [shifting left by 1 lower digit multiplication]
    //                    -------
    // (sum of products)     1353   [then add the result]
    
    // Determine the larger buffer, this must be the top number for efficiency.
    bool lhsBigger = (_magnitude.size() > rhs._magnitude.size());
    auto& topOperandBuffer = (lhsBigger) ? _magnitude : rhs._magnitude;
    auto& bottomOperandBuffer = (lhsBigger) ? rhs._magnitude : _magnitude;
    
    // The product accumulator is used to accumulate the multiple additions: 1 for each bottom operand digit.
    //  Reserve enough memory to hold 2*<largest operand> so no reallocation of this buffer is necessary.
    BigInteger productAccumulator = 0;
    productAccumulator._magnitude.reserve(topOperandBuffer.size() * 2);
    
    // The individual product is used to hold the result of each multiplication of a
    //  bottom digit by all of the top digits. We will deal with this one mostly by its vector.
    //  We will with it backwards and reverse it at the end. Simplifies indexing.
    BigInteger individualProduct = 0;
    auto& individualProductBuffer = individualProduct._magnitude;
    
    // This counter gets incremented for each bottom digit multiplied to track the shift in the
    //  addition.
    int individualProductStartingIndex = 0;
    
    // Outer loop: iterate over the bottom operand digits in reverse.
	int bottomOperandIndex = static_cast<int>(bottomOperandBuffer.size()) - 1;

    while(bottomOperandIndex >= 0)
    {
        // Prepare the individual product buffer. it should be large enough to hold a full multiplication.
        //  It should be as large as necessary and filled with zero (it will never resize smaller).
        individualProductBuffer.resize(topOperandBuffer.size() * 2, 0);
        fill(individualProductBuffer.begin(), individualProductBuffer.end(), 0);
        
        // Save away and increment the starting point for the individual product buffer.
        int individualProductIndex = individualProductStartingIndex++;
        
        // Inner loop: iterate over the top operand digits in reverse for each bottom operand digit.
		int topOperandIndex = static_cast<int>(topOperandBuffer.size()) - 1;
        while(topOperandIndex >= 0)
        {
            // Do the operation on a single digit.
            uint8_t currentTopOperand = topOperandBuffer[topOperandIndex];
            uint8_t currentBottomOperand = bottomOperandBuffer[bottomOperandIndex];
            uint16_t currentProduct = currentTopOperand * currentBottomOperand;
            
            // Split the short into a high-byte and low-byte.
            uint8_t lowByte = (currentProduct & 0x00FF);
            uint8_t highByte = (currentProduct & 0xFF00) >> 8;
            
            // Each multiplication can affect maximum of 3 digits: the target digit, the next (for the high-byte),
            //  and one more for addition overflow.
            uint16_t calculatedLowByte = individualProductBuffer[individualProductIndex] + lowByte;
            uint16_t calculatedHighByte = individualProductBuffer[individualProductIndex + 1] + highByte + ((calculatedLowByte & 0xFF00) >> 8);
            uint8_t calculatedOverflowByte = (calculatedHighByte & 0xFF00) >> 8;
            
            // Set all these.
            individualProductBuffer[individualProductIndex] = (calculatedLowByte & 0x00FF);
            individualProductBuffer[individualProductIndex + 1] = (calculatedHighByte & 0x00FF);
            if(calculatedOverflowByte != 0)
				individualProductBuffer[individualProductIndex + 2] = calculatedOverflowByte;
            
            // Decrement all iterators/indices.
            --topOperandIndex;
            individualProductIndex++;
        }
        
        // Prepare individual product for addition (fix byte order, remove extra zeros, etc.)
        std::reverse(individualProductBuffer.begin(), individualProductBuffer.end());
        individualProduct.TrimPrefixZeros();
        productAccumulator += individualProduct;
        
        --bottomOperandIndex;
    }
    
    // The switch updates the "this" reference.
    _magnitude.swap(productAccumulator._magnitude);
    
    return *this;
}

BigInteger& BigInteger::Subtract(const BigInteger& rhs)
{
    // This function does not support a subtraction resulting in a negative number.
    //  It will only subtract a smaller magnitude from a larger.
    if(CompareMagnitudeTo(rhs) == -1)
    {
        stringstream ss;
        ss << "Negative result of subtraction not supported. Attempted operation: ";
        ss << ToString() << " - " << rhs.ToString();
        throw invalid_argument(ss.str());
    }
    
    // The operation will be done as a long-hand subtraction.
    // Example:
    //     (topNumber)      42345   [the larger of the two]
    //  (bottomNumber)    - 15678   [the smaller of the two]
    //                    -------
    //    (difference)      2cccd
    // A difference buffer and a isBorrowing flag will keep track of results.
    
    // The buffer may get smaller, but will not get larger.
    //  Note that we are adding the bytes to the difference buffer "backwards"
    //  to simplify determining empty digits.
    vector<uint8_t> differenceBuffer;
    differenceBuffer.resize(_magnitude.size());
    
    // Iterating through the top and bottom backwards.
    int lhsIndex = static_cast<int>(_magnitude.size()) - 1;
    int rhsIndex = static_cast<int>(rhs._magnitude.size()) - 1;
	int differenceIndex = static_cast<int>(differenceBuffer.size()) - 1;

    while(lhsIndex >= 0)
    {
        // Pull out the current top (aways there) and the current bottom (or zero if no more digits).
        uint8_t currentTop = _magnitude[lhsIndex];
        uint8_t currentBottom = (rhsIndex >= 0) ? rhs._magnitude[rhsIndex] : 0;
        
        // Do the subtraction in a signed variable. This may result in a negative number.
        int currentDifference = currentTop - currentBottom;
        
        // Borrow from the next if necessary.
        if(currentDifference < 0)
        {
            Borrow(_magnitude, lhsIndex - 1);
            currentDifference += 0x100;
        }
        
        // Put the (now guaranteed positive) difference, masked to ensure proper size
        //  in the difference buffer.
        differenceBuffer[differenceIndex] = static_cast<unsigned short>(currentDifference) & 0xFF;
        
        // Increment everything.
        --lhsIndex;
		--rhsIndex;
		--differenceIndex;
    }
    
    // Since we created the difference backwards, reverse it then set it to the buffer.
    _magnitude.swap(differenceBuffer);
    
    // Remove any empty zero bytes from the beginning of the buffer.
    TrimPrefixZeros();
    
    return *this;
}

// Divides the numerator by the divisor, returns the quotient and remainder as a pair.
pair<BigInteger, BigInteger> BigInteger::Divide(const BigInteger& numerator, const BigInteger& divisor)
{
    // Divide using the following bitwise division algorithm.
    //  Algorithm found here (long division algorithm): http://en.wikipedia.org/wiki/Division_algorithm
    //
    // if D == 0 then throw DivisionByZeroException end
    // Q := 0                 initialize quotient and remainder to zero
    // R := 0
    // for i = n-1...0 do     where n is number of bits in N
    //     R := R << 1          left-shift R by 1 bit
    //     R(0) := N(i)         set the least-significant bit of R equal to bit i of the numerator
    //     if R >= D then
    //         R = R - D
    //         Q(i) := 1
    //     end
    // end
    
    // Note that this division operation ignores sign and only deals in magnitude.
    BigInteger quotient(0);
    BigInteger remainder(0);
    
    for(int i = static_cast<int>(numerator.GetBitSize()) - 1; i >= 0; --i)
    {
        remainder <<= 1;
        if(numerator.GetBitAt(i)) remainder.SetBitAt(0);
        quotient <<= 1;
        if(remainder.CompareMagnitudeTo(divisor) >= 0)
        {
            remainder.Subtract(divisor);
            quotient.SetBitAt(0);
        }
    }
    
    return pair<BigInteger, BigInteger>(move(quotient), move(remainder));
}

// Binary addition operator with BigIntegers. Declared as free function by convention.
BigInteger operator+(BigInteger lhs, const BigInteger& rhs)
{
    lhs += rhs;
    return lhs;
}

// Binary subtraction operator with BigIntegers. Declared as free function by convention.
BigInteger operator-(BigInteger lhs, const BigInteger& rhs)
{
    lhs -= rhs;
    return lhs;
}

// Binary multiplication operator with BigIntegers. Declared as free function by convention.
BigInteger operator*(BigInteger lhs, const BigInteger& rhs)
{
    lhs *= rhs;
    return lhs;
}

// Binary modulus operator with BigIntegers. Declared as free function by convention.
BigInteger operator/(BigInteger lhs, const BigInteger& rhs)
{
    lhs /= rhs;
    return lhs;
}

// Binary modulus operator with BigIntegers. Declared as free function by convention.
BigInteger operator%(BigInteger lhs, const BigInteger& rhs)
{
    lhs %= rhs;
    return lhs;
}

// Overload of the absolute value function abs() for BigInteger.
BigInteger abs(const BigInteger& bigInteger)
{
    return (bigInteger >= 0) ? bigInteger : -bigInteger;
}

// Overload of out stream operator to print a representation of the BigInteger.
std::ostream& operator<<(std::ostream& os, const BigInteger& bigInteger)
{
    os << bigInteger.ToString();
    return os;
}



















