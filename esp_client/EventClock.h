// EventClock.h

#ifndef EVENTCLOCK_H
#define EVENTCLOCK_H

#include <Arduino.h>

class EventClock {
public:
  EventClock();
  void fetchEventFromServer();
  void update();
  long getWeeks();
  long getDays();
  long getHours();
  long getMinutes();
  bool hasEvent();

private:
  long _eventEpoch;
  long _now;
};

#endif