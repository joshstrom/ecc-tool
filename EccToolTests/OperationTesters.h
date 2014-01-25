//
//  BaseOperationTester.h
//  EccTool
//
//  Created by Josh Strom on 1/21/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_OperationTesters_h
#define EccTool_OperationTesters_h

#include "Stopwatch.h"


void RunAdditionTest(const int number1, const int number2);
void RunSubtractionTest(const int number1, const int number2);
void RunMultiplicationTest(const int number1, const int number2);
void RunDivisionTest(const int number1, const int number2);
void RunModulusTest(const int number1, const int number2);
void RunLTTest(const int number1, const int number2);
void RunGTTest(const int number1, const int number2);
void RunLEQTest(const int number1, const int number2);
void RunGEQTest(const int number1, const int number2);
void RunEQTest(const int number1, const int number2);
void RunNEQTest(const int number1, const int number2);

class BaseOperationTester
{
protected:
    virtual int SelectFirstOperand(int number1, int number2) const
    {
        return number1; // Default implementation.
    }
    
    virtual int SelectSecondOperand(int number1, int number2) const
    {
        return number2; // Default implementation.
    }
    
    void StartTimer(Stopwatch* const sw) const
    {
        if(sw == nullptr)
            return;
        sw->Start();
    }
    
    void StopTimer(Stopwatch* const sw) const
    {
        if(sw == nullptr)
            return;
        sw->Stop();
    }
    
public:
    
    virtual string GetOperationSymbol() const = 0;
    virtual void TestOperation(int number1, int number2, Stopwatch* const sw, int count = -1) const = 0;
};

class BaseMathOperationTester : public BaseOperationTester
{
protected:
    virtual BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const = 0;
    virtual long long RunPrimitiveOperation(const int operand1, const int operand2) const = 0;
    
public:
    void TestOperation(int number1, int number2, Stopwatch* const sw, int count = -1) const override
    {
        auto operand1 = SelectFirstOperand(number1, number2);
        auto operand2 = SelectSecondOperand(number1, number2);
        
        auto primitiveOperationResult = BigInteger(RunPrimitiveOperation(operand1, operand2));

        BigInteger bigIntegerOperand1(operand1);
        BigInteger bigIntegerOperand2(operand2);
        
        StartTimer(sw);
        auto bigIntegerOperationResult = RunBigIntegerOperation(bigIntegerOperand1, bigIntegerOperand2);
        StopTimer(sw);
        
        if(primitiveOperationResult != bigIntegerOperationResult)
        {
            stringstream ss;
            ss  << "Operation " << BigInteger(operand1).ToString() << " " << GetOperationSymbol()
                << " " << BigInteger(operand2).ToString() << " resulted in " << bigIntegerOperationResult.ToString()
                << ". Should have been: " << primitiveOperationResult.ToString();
            
            if(count != -1)
                ss << ", Counter: " << count;
            
            FAIL(ss.str());
        }
    }
};

class AdditionOperationTester final : public BaseMathOperationTester
{
protected:
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 + operand2;
    }
    
    long long RunPrimitiveOperation(const int operand1, const int operand2) const override
    {
        return operand1 + operand2;
    }

public:
    string GetOperationSymbol() const override
    {
        return "+";
    }
};

class MultiplicationOperationTester final : public BaseMathOperationTester
{
protected:
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 * operand2;
    }
    
    long long RunPrimitiveOperation(const int operand1, const int operand2) const override
    {
        return static_cast<long long>(operand1) * static_cast<long long>(operand2);
    }

public:
    string GetOperationSymbol() const override
    {
        return "*";
    }
};

class SubtractionOperationTester final : public BaseMathOperationTester
{
protected:
   
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 - operand2;
    }
    
    long long RunPrimitiveOperation(const int operand1, const int operand2) const override
    {
        return operand1 - operand2;
    }
public:
    string GetOperationSymbol() const override
    {
        return "-";
    }
};

class DivisionOperationTester final : public BaseMathOperationTester
{
protected:
    
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 / operand2;
    }

    long long RunPrimitiveOperation(const int operand1, const int operand2) const override
    {
        return operand1 / operand2;
    }
public:
    string GetOperationSymbol() const override
    {
        return "/";
    }
};

class ModulusOperationTester final : public BaseMathOperationTester
{
protected:
    
    BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const override
    {
        return operand1 % operand2;
    }

    long long RunPrimitiveOperation(const int operand1, const int operand2) const override
    {
        return abs(operand1 % operand2); // BigInteger modulo operations always result in positive numbers.
    }
public:
    string GetOperationSymbol() const override
    {
        return "%";
    }
};

class BaseComparisonOperationTester : public BaseOperationTester
{
public:
    virtual void TestOperation(int number1, int number2, Stopwatch* const sw = nullptr, int count = -1) const
    {
        auto operand1 = SelectFirstOperand(number1, number2);
        auto operand2 = SelectSecondOperand(number1, number2);
        
        bool primitiveResult = RunPrimitiveComparisonOperation(operand1, operand2);
        
        BigInteger bigInterOperand1(operand1);
        BigInteger bigIntegerOperand2(operand2);
        
        StartTimer(sw);
        bool bigIntegerComparisonResult = RunBigIntegerComparisonOperation(bigInterOperand1, bigIntegerOperand2);
        StopTimer(sw);
        
        if(primitiveResult == bigIntegerComparisonResult)
            return;
        
        stringstream failMessage;
        failMessage << hex;
        failMessage << "Comparison " << operand1 << GetOperationSymbol() << operand2 << " failed.";
        
        if(count != -1)
            failMessage << ", Counter: " << count;
        
        FAIL(failMessage.str());
    }

protected:
    virtual bool RunPrimitiveComparisonOperation(int operand1, int operand2) const = 0;
    virtual bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const = 0;
};

class LTOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 < operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 < operand2;
    }

public:
    string GetOperationSymbol() const override
    {
        return "<";
    }
};

class GTOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 > operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 > operand2;
    }
    
public:
    string GetOperationSymbol() const override
    {
        return ">";
    }
};

class LEQOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 <= operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 <= operand2;
    }
    
public:
    string GetOperationSymbol() const override
    {
        return "<=";
    }
};

class GEQOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 >= operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 >= operand2;
    }
    
public:
    string GetOperationSymbol() const override
    {
        return ">=";
    }
};

class EQOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 == operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 == operand2;
    }
    
public:
    string GetOperationSymbol() const override
    {
        return "==";
    }
};

class NEQOperationTester final : public BaseComparisonOperationTester
{
protected:
    bool RunPrimitiveComparisonOperation(int operand1, int operand2) const
    {
        return operand1 != operand2;
    }
    
    bool RunBigIntegerComparisonOperation(const BigInteger& operand1, const BigInteger& operand2) const
    {
        return operand1 != operand2;
    }
    
public:
    string GetOperationSymbol() const override
    {
        return "!=";
    }
};

class AllComparisonOperationTester final : public BaseOperationTester
{
public:
    void TestOperation(int number1, int number2, Stopwatch* const sw, int count = -1) const override
    {
        RunLTTest(number1, number2);
        RunGTTest(number1, number2);
        RunLEQTest(number1, number2);
        RunGEQTest(number1, number2);
        RunEQTest(number1, number2);
        RunNEQTest(number1, number2);
    }

    string GetOperationSymbol() const override
    {
        return "";
    }
};

void RunAdditionTest(const int number1, const int number2)
{
    AdditionOperationTester().TestOperation(number1, number2, nullptr);
}

void RunSubtractionTest(const int number1, const int number2)
{
    SubtractionOperationTester().TestOperation(number1, number2, nullptr);
}

void RunMultiplicationTest(const int number1, const int number2)
{
    MultiplicationOperationTester().TestOperation(number1, number2, nullptr);
}

void RunDivisionTest(const int number1, const int number2)
{
    DivisionOperationTester().TestOperation(number1, number2, nullptr);
}

void RunModulusTest(const int number1, const int number2)
{
    ModulusOperationTester().TestOperation(number1, number2, nullptr);
}

void RunLTTest(const int number1, const int number2)
{
    LTOperationTester().TestOperation(number1, number2, nullptr);
}

void RunGTTest(const int number1, const int number2)
{
    GTOperationTester().TestOperation(number1, number2, nullptr);
}

void RunLEQTest(const int number1, const int number2)
{
    LEQOperationTester().TestOperation(number1, number2, nullptr);
}

void RunGEQTest(const int number1, const int number2)
{
    GEQOperationTester().TestOperation(number1, number2, nullptr);
}

void RunEQTest(const int number1, const int number2)
{
    EQOperationTester().TestOperation(number1, number2, nullptr);
}

void RunNEQTest(const int number1, const int number2)
{
    NEQOperationTester().TestOperation(number1, number2, nullptr);
}

#endif
