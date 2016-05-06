#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


void setup() {
  Serial.begin(9600);

  // Initialize the lcd for 20 chars 4 lines, turn on backlight
  lcd.begin(20, 4);

  // Blink backlight 3 times
  for (int i = 0; i < 3; i++) {
    lcd.noBacklight();
    delay(250);
    lcd.backlight();
    delay(250);
  }

}

void loremIpsum() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lorem ipsum dolor ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("sit amet, sed do ");
  delay(1000);
  lcd.setCursor(0, 2);
  lcd.print("eiusmod tempor ");
  delay(1000);
  lcd.setCursor(0, 3);
  lcd.print("incididunt ut labore");
  delay(2000);
}

void loremIpsumScrolling() {
  lcd.clear();
  String lorem = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod";

  lcd.setCursor(0, 0);
  lcd.print("Scrolling example:");

  lcd.setCursor(0, 2);
  lcd.print(lorem.substring(0, 20));
  delay(2000);

  for (int i = 0; i < lorem.length(); i++) {
    int end = min(i + 20, lorem.length());

    char substr[end - i + 1];
    char line[21];

    lorem.substring(i, end + 1).toCharArray(substr, end - i + 1);
    sprintf(line, "%-20s", substr);

    lcd.setCursor(0, 2);
    lcd.print(line);

    delay(200);
  }
}

void loop() {
  loremIpsum();
  loremIpsumScrolling();
}

