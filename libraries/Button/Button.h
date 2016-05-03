#include "Arduino.h"
#ifndef Button_h
#define Button_h

class Button
{
  private:
    int lastValue;
    unsigned long lastDebounceTime;  
    void (*pCallback)(Button*);
  public:
	String label;
    int pin;
	int state;
    bool pressed;
    void read();
    Button(int PIN, void (*pCallbackFunction)(Button*));
};

#endif
