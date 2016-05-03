#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Button.h"

// RGB LED
int red = 8;
int green = 9;
int blue = 10;

// LCD screen
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
Button button = Button(2, &toggle_backlight_callback);
bool lcd_backlight_on = true;

// Moisture sensor
int analog = A0;
int limit_too_much = 200;
int limit_too_little = 500;
unsigned long last_sample_time = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(analog, INPUT);
  pinMode(red,    OUTPUT);
  pinMode(green,  OUTPUT);
  pinMode(blue,   OUTPUT);

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

void toggle_backlight_callback(Button* button) {
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

void plant_too_wet() {
  rgb(LOW, LOW, HIGH);
  lcd.setCursor(0,3);
  lcd.print("Woah, too much!");
}

void plant_is_good() {
  rgb(LOW, HIGH, LOW);
  lcd.setCursor(0,3);
  lcd.print("All is good!");
}

void plant_too_dry() {
  rgb(HIGH, LOW, LOW);
  lcd.setCursor(0,3);
  lcd.print("Feed me!");
}

void loop() {
  if (millis() - last_sample_time > 1000) {
    last_sample_time = millis();
    
    int analog = analogRead(A0);
    Serial.println(analog);
    
    lcd.setCursor(16,1);
    lcd.print(analog);
    
    if (analog < limit_too_much) {
      plant_too_wet();
    } else if (analog < limit_too_little) {
      plant_is_good();
    } else {
      plant_too_dry();
    }
  }

  button.read();
}

