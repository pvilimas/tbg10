#ifndef __TIMER__
#define __TIMER__

#include <functional>

#include "raylib.h"

/*
    small utility class that returns true every x seconds (ignoring any granularity with how often it's called)
    has an optional callback that can run after the interval passed
*/
class Timer {
    private:
        
    double interval;
    double lastRecorded;
    std::function<void()> callback;

    public:

    static const std::function<void()> defaultCallback;

    /*
        Timer constructor
        _interval - the interval
        _callback - the optional callback
    */
    Timer(double _interval, std::function<void()> _callback = defaultCallback);
    
    /*  returns true if an interval has passed, then resets the interval  */
    bool IntervalPassed();
    
    /*  checks interval, calls the callback if needed  */
    void CheckTime();
    
    /*  potential BUG here idk, maybe make it set to GetTime instead of 0  */
    void Reset();
};

#endif /* __TIMER__ */