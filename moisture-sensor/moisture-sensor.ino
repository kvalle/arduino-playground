// kveld 1: 220
// kveld 2: 316

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int wait = 10000;

int red = 8;
int green = 9;
int blue = 10;

int digital = 2;
int analog = A0;

int limit_too_much = 200;
int limit_too_little = 500;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void setup() {
  pinMode(A0, INPUT); // A0

  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);

  Serial.begin(9600);

  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Audrey Jr.");
  lcd.setCursor(0,1);
  lcd.print("Moisture level:");
}

void rgb(int r, int g, int b) {
  digitalWrite(red, r);
  digitalWrite(green, g);
  digitalWrite(blue, b);
}

void loop() {
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

  delay(wait);
}

