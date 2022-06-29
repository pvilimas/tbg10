#include "timer.hpp"

const std::function<void()> Timer::defaultCallback = []{};

Timer::Timer(double _interval, std::function<void()> _callback) {
    interval = _interval;
    lastRecorded = 0;
    callback = _callback;
}

bool Timer::IntervalPassed() {
    double newTime = GetTime();
    if (lastRecorded + interval <= newTime) {
        lastRecorded = newTime;
        return true;
    }
    return false;
}

void Timer::CheckTime() {
    if (IntervalPassed()) {
        callback();
    }
}

void Timer::Reset() {
    lastRecorded = 0;
}