#include "Button.h"

//Declare two buttons on pin 2 and 3, pass the address (&) of the callback functions
Button myButton = Button(2, &myCallback);
Button myOtherButton = Button(3, &myOtherCallback);

//Keep pointers to all the buttons in an array for simpler handling of many buttons
const int buttonAmount = 2;
Button* buttons[buttonAmount] = {
  &myButton,
  &myOtherButton
};

void setup() {
  Serial.begin(9600);
}

//Read the buttons in the loop
void loop() {
  for (int i = 0; i < buttonAmount; i++){
    buttons[i]->read();
  }
}

void myCallback(Button* button)
{
  if (button->pressed){
    Serial.println("pressed button 1");
  }
  else{
    Serial.println("released button 1");
  }
}

void myOtherCallback(Button* button)
{
  if (button->pressed){
    Serial.println("pressed button 2");
  }
  else{
    Serial.println("released button 2");
  }
}
