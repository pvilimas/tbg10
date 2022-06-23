#ifndef __TIMER__
#define __TIMER__

#include <functional>

#include "raylib.h"

class Timer {
    private:
        
    double interval;
    double lastRecorded;
    std::function<void()> callback;

    public:

    static const std::function<void()> defaultCallback;

    Timer(double _interval, std::function<void()> _callback = defaultCallback);
    bool IntervalPassed();
    void CheckTime();
    void Reset();
};

#endif /* __TIMER__ */