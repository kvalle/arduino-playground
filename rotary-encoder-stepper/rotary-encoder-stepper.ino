#include "Stepper.h"

// Encoder variables
int encoderPinA = 2;
int encoderPinB = 3;
int encoderPos = 0;
int encoderPinALast = LOW;
int val = LOW;

// Stepper motor variables
Stepper stepper(32, 8, 9, 10, 11); // 32 steps, IN1-4 pins
int stepperPos;

void setup() {
  Serial.begin(9600);

  // Encoder setup
  pinMode (encoderPinA, INPUT);
  pinMode (encoderPinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doCallback, CHANGE);

  // Stepper setup
  stepper.setSpeed(600);
}

void loop() {  
  if (stepperPos != encoderPos) {
    int diff = encoderPos - stepperPos;
    Serial.println(diff);
    stepper.step(100 * diff);
    stepperPos = encoderPos;
  }
}

void doCallback() {
  val = digitalRead(encoderPinA);
  if (encoderPinALast == LOW && val == HIGH) {
    if (digitalRead(encoderPinB) == LOW) {
      encoderPos--;
    } else {
      encoderPos++;
    }
    //Serial.println(encoderPos);
  }
  encoderPinALast = val;
}

