#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Button.h"

int wait = 100;

int red = 8;
int green = 9;
int blue = 10;

Button button = Button(2, &toggle_backlight);
int analog = A0;

bool lcd_backlight_on = true;

int limit_too_much = 200;
int limit_too_little = 500;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void setup() {
  Serial.begin(9600);
  
  pinMode(A0, INPUT); // A0
  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);

  setup_lcd();
}

void setup_lcd() {
  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Audrey Jr.");
  lcd.setCursor(0,1);
  lcd.print("Moisture level:");

  lcd.backlight();
}

void toggle_backlight(Button* button) {
  if (button->pressed){
    lcd_backlight_on = !lcd_backlight_on;

    if (lcd_backlight_on) {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
  }
}


void rgb(int r, int g, int b) {
  digitalWrite(red, r);
  digitalWrite(green, g);
  digitalWrite(blue, b);
}

int i = 0;

void loop() {
  i = i+1;
  
  if (i > 10) {
    int analog = analogRead(A0);
    Serial.println(analog);
    lcd.setCursor(16,1);
    lcd.print(analog);
    
    if (analog < limit_too_much) {
      // [0,199] -> too much water, blue light.
      rgb(LOW, LOW, HIGH);
      lcd.setCursor(0,3);
      lcd.print("Woah, too much!");
    } else if (analog < limit_too_little) {
      // [200, 499] -> water si good, green light.
      rgb(LOW, HIGH, LOW);
      lcd.setCursor(0,3);
      lcd.print("All is good!");
    } else {
      // [500,1023] -> too little water, red light.
      rgb(HIGH, LOW, LOW);
      lcd.setCursor(0,3);
      lcd.print("Feed me!");
    }
    i = 0;
  }

  button.read();

  delay(wait);
}

