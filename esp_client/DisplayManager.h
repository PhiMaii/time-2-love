#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>



class DisplayManager {
public:
  DisplayManager();
  void begin();
  void updateDisplay(long weeks, long days, long hours, long minutes, int rssi, bool serverUp);
  void showTempMessage(const char* msg);
  void startBlinking(unsigned long ms);
  static void displayUpdateProgress(int progress, int total);

private:
  unsigned long _blinkUntil;
  unsigned long _tempUntil;
  String _tempMsg;
  unsigned long _lastDrawMs;
};

#endif