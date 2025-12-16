#include "DelayedCaller.h"

DelayedCaller::DelayedCaller()
{
    _callback = nullptr;
    _delay = 0;
    _startTime = 0;
    _active = false;
}

void DelayedCaller::callWithDelay(void (*callback)(), unsigned long delayMs)
{
    _callback = callback;      // store the function pointer
    _delay = delayMs;          // store the delay
    _startTime = millis();     // start the timer
    _active = true;            // mark timer as active
}

void DelayedCaller::update()
{
    if (_active && millis() - _startTime >= _delay)
    {
        if (_callback) _callback(); // call the scheduled function
        _active = false;            // reset for one-time use
    }
}