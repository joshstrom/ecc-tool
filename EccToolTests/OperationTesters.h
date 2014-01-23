//
//  BaseOperationTester.h
//  EccTool
//
//  Created by Josh Strom on 1/21/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_OperationTesters_h
#define EccTool_OperationTesters_h


void RunAdditionTest(const int number1, const int number2);
void RunSubtractionTest(const int number1, const int number2);
void RunMultiplicationTest(const int number1, const int number2);
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
    
public:
    virtual string GetOperationSymbol() const = 0;
    virtual void TestOperation(int number1, int number2, int count = -1) const = 0;
};

class BaseMathOperationTester : public BaseOperationTester
{
protected:
    virtual BigInteger RunBigIntegerOperation(const BigInteger& operand1, const BigInteger& operand2) const = 0;
    virtual long long RunPrimitiveOperation(const int operand1, const int operand2) const = 0;
    
public:
    void TestOperation(int number1, int number2, int count = -1) const override
    {
        auto operand1 = SelectFirstOperand(number1, number2);
        auto operand2 = SelectSecondOperand(number1, number2);
        
        auto primitiveOperationResult = BigInteger(RunPrimitiveOperation(operand1, operand2));

        auto bigIntegerOperationResult = RunBigIntegerOperation(BigInteger(operand1), BigInteger(operand2));
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
    int SelectFirstOperand(int number1, int number2) const override
    {
        return (number1 > number2) ? number1 : number2; // Largest.
    }
    
    int SelectSecondOperand(int number1, int number2) const override
    {
        return (number1 > number2) ? number2 : number1; // Smallest.
    }
    
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

class BaseComparisonOperationTester : public BaseOperationTester
{
public:
    virtual void TestOperation(int number1, int number2, int count = -1) const
    {
        auto operand1 = SelectFirstOperand(number1, number2);
        auto operand2 = SelectSecondOperand(number1, number2);
        
        bool primitiveResult = RunPrimitiveComparisonOperation(operand1, operand2);
        bool bigIntegerComparisonResult = RunBigIntegerComparisonOperation(operand1, operand2);
        
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
    void TestOperation(int number1, int number2, int count = -1) const override
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
    AdditionOperationTester().TestOperation(number1, number2);
}

void RunSubtractionTest(const int number1, const int number2)
{
    SubtractionOperationTester().TestOperation(number1, number2);
}

void RunMultiplicationTest(const int number1, const int number2)
{
    MultiplicationOperationTester().TestOperation(number1, number2);
}

void RunLTTest(const int number1, const int number2)
{
    LTOperationTester().TestOperation(number1, number2);
}

void RunGTTest(const int number1, const int number2)
{
    GTOperationTester().TestOperation(number1, number2);
}

void RunLEQTest(const int number1, const int number2)
{
    LEQOperationTester().TestOperation(number1, number2);
}

void RunGEQTest(const int number1, const int number2)
{
    GEQOperationTester().TestOperation(number1, number2);
}

void RunEQTest(const int number1, const int number2)
{
    EQOperationTester().TestOperation(number1, number2);
}

void RunNEQTest(const int number1, const int number2)
{
    NEQOperationTester().TestOperation(number1, number2);
}

#endif
