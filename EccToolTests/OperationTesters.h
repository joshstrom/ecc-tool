//
//  BaseOperationTester.h
//  EccTool
//
//  Created by Josh Strom on 1/21/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_OperationTesters_h
#define EccTool_OperationTesters_h

class BaseOperationTester
{
protected:
    virtual BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const = 0;
    virtual unsigned long long RunPrimitiveOperation(const unsigned int operand1, const unsigned int operand2) const = 0;
    
    virtual unsigned int SelectFirstOperand(unsigned int number1, unsigned int number2) const
    {
        return number1; // Default implementation.
    }
    
    virtual unsigned int SelectSecondOperand(unsigned int number1, unsigned int number2) const
    {
        return number2; // Default implementation.
    }
    
public:
    virtual char GetOperationSymbol() const = 0;
    
    void TestOperation(unsigned int number1, unsigned int number2, int count = -1) const
    {
        auto operand1 = SelectFirstOperand(number1, number2);
        auto operand2 = SelectSecondOperand(number1, number2);
        
        auto primitiveOperationResult = RunPrimitiveOperation(operand1, operand2);
        
        stringstream ss;
        ss << hex << primitiveOperationResult;
        
        string primitiveOperationResultInHex = ss.str();
        ss.str("");
        ss.clear();
        
        if((primitiveOperationResultInHex.size() % 2) != 0)
            primitiveOperationResultInHex.insert(primitiveOperationResultInHex.begin(), '0');
        
        if(count != -1)
            ss << ", Counter: " << count;
        
        string bigIntegerOpResultInHex = RunBigIntegerOperation(BigInteger(operand1), BigInteger(operand2)).ToString();
        if(primitiveOperationResultInHex != bigIntegerOpResultInHex)
            FAIL("Operation " << hex << operand1 << GetOperationSymbol() << operand2 << " resulted in " << bigIntegerOpResultInHex << ". Should have been: " << primitiveOperationResultInHex << ss.str());
    }
    
};

class AdditionOperationTester final : public BaseOperationTester
{
protected:
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 + operand2;
    }
    
    unsigned long long RunPrimitiveOperation(const unsigned int operand1, const unsigned int operand2) const override
    {
        return operand1 + operand2;
    }

public:
    char GetOperationSymbol() const override
    {
        return '+';
    }
};

class MultiplicationOperationTester final : public BaseOperationTester
{
protected:
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 * operand2;
    }
    
    unsigned long long RunPrimitiveOperation(const unsigned int operand1, const unsigned int operand2) const override
    {
        return static_cast<unsigned long long>(operand1) * static_cast<unsigned long long>(operand2);
    }

public:
    char GetOperationSymbol() const override
    {
        return '*';
    }
};

class SubtractionOperationTester final : public BaseOperationTester
{
protected:
    unsigned int SelectFirstOperand(unsigned int number1, unsigned int number2) const override
    {
        return (number1 > number2) ? number1 : number2; // Largest.
    }
    
    unsigned int SelectSecondOperand(unsigned int number1, unsigned int number2) const override
    {
        return (number1 > number2) ? number2 : number1; // Smallest.
    }
    
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 - operand2;
    }
    
    unsigned long long RunPrimitiveOperation(const unsigned int operand1, const unsigned int operand2) const override
    {
        return operand1 - operand2;
    }
public:
    char GetOperationSymbol() const override
    {
        return '-';
    }
};

void RunAdditionTest(const unsigned int number1, const unsigned int number2)
{
    AdditionOperationTester().TestOperation(number1, number2);
}

void RunSubtractionTest(const unsigned int number1, const unsigned int number2)
{
    SubtractionOperationTester().TestOperation(number1, number2);
}

void RunMultiplicationTest(const unsigned int number1, const unsigned int number2)
{
    MultiplicationOperationTester().TestOperation(number1, number2);
}

#endif
