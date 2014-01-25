//
//  Stopwatch.h
//  EccTool
//
//  Created by Josh Strom on 1/24/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__Stopwatch__
#define __EccTool__Stopwatch__

#include <iostream>
#include <exception>

using namespace std;

class Stopwatch
{
private:
    
    unsigned long GetCurrentTime() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    unsigned long _storedTime;
    bool _isRunning;
    unsigned long _elapsedTime;
    
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
    
    unsigned long GetElapsedTime() const
    {
        if(_isRunning)
            throw runtime_error("Cannot get elapsed time while Stopwatch is running.");
        return _elapsedTime;
    }
};

#endif /* defined(__EccTool__Stopwatch__) */
