#include "LedControl.h"
#include "Button.h"

/*
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);
Button myButton = Button(8, &myCallback);
long counter = 42;

void setup() {
  Serial.begin(9600);
  
  lc.shutdown(0,false);
  lc.setIntensity(0,5);
  lc.clearDisplay(0);

  foo(counter);
}

void myCallback(Button* button)
{
  if (button->pressed){
    foo(++counter);
  }
}

void foo(long num) {
  if (num > 99999999L) {
    num = 99999999L;
  }

  byte digit = 0;
  while (true) {
    Serial.println(num % 10);
    lc.setDigit(0, digit++, num % 10, false);
    num /= 10;
    if (num <= 0) break;
  }
}

void loop() {
  myButton.read();
}



