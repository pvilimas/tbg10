#include "timer.hpp"

const std::function<void()> Timer::defaultCallback = []{};

Timer::Timer(double _interval, std::function<void()> _callback) {
    interval = _interval;
    lastRecorded = 0;
    callback = _callback;
}

/* returns true if an interval has passed, then resets the interval */
bool Timer::IntervalPassed() {
    double newTime = GetTime();
    if (lastRecorded + interval <= newTime) {
        lastRecorded = newTime;
        return true;
    }
    return false;
}

/* checks interval, calls the callback if needed */
void Timer::CheckTime() {
    if (IntervalPassed()) {
        callback();
    }
}

/*
    potential bug here idk, maybe make it set to GetTime instead of 0
*/
void Timer::Reset() {
    lastRecorded = 0;
}