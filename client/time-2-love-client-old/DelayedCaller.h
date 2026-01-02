#ifndef DELAYEDCALLER_H
#define DELAYEDCALLER_H

#pragma once
#include <Arduino.h>

class DelayedCaller
{
public:
    DelayedCaller();

    // Schedule any function to run after delayMs milliseconds
    void callWithDelay(void (*callback)(), unsigned long delayMs);

    // Must be called in loop() to check the timer
    void update();

private:
    void (*_callback)();      // function pointer
    unsigned long _delay;     // delay in ms
    unsigned long _startTime; // when the timer started
    bool _active;             // is timer active?
};

#endif