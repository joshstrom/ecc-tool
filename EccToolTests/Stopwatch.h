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
#ifndef __EccTool__Stopwatch__
#define __EccTool__Stopwatch__

#include <iostream>
#include <exception>
#include <chrono>
#include <ctime>

using namespace std;

class Stopwatch
{
private:
    
    long long GetCurrentTime() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    long long _storedTime;
    bool _isRunning;
    long long _elapsedTime;
    
public:
    Stopwatch() : _storedTime(0), _elapsedTime(0)
    {
    }
    
    void Start()
    {
        if(_isRunning)
            throw runtime_error("Stopwatch already running.");

        _isRunning = true;
        _storedTime = GetCurrentTime();
    }
    
    void Stop()
    {
        auto stopTime = GetCurrentTime();
        
        if(!_isRunning)
            throw runtime_error("Stopwatch not running.");
        
        _isRunning = false;
        _elapsedTime += (stopTime - _storedTime);
        _storedTime = 0;
    }
    
    void Reset()
    {
        _isRunning = false;
        _elapsedTime = false;
        _storedTime = false;
    }
    
    long long GetElapsedTime() const
    {
        if(_isRunning)
            throw runtime_error("Cannot get elapsed time while Stopwatch is running.");
        return _elapsedTime;
    }
};

#endif /* defined(__EccTool__Stopwatch__) */
