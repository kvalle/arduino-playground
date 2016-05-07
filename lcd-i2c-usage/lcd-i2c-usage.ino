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

void norwegianLetters() {
  // Create using https://omerk.github.io/lcdchargen/

  byte hearthFull[8]  = { B00000,B00000,B01010,B11111,B11111,B01110,B00100,B00000 };
  byte hearthEmpty[8] = { B00000,B00000,B01010,B10101,B10001,B01010,B00100,B00000 };
  byte lowerAe[8]     = { B00000,B00000,B11010,B00101,B01111,B10100,B11111,B00000 }; // æ
  byte lowerOe[8]     = { B00000,B00001,B01110,B10101,B10101,B01110,B10000,B00000 }; // ø
  byte lowerAa[8]     = { B00100,B00000,B01110,B00001,B01111,B10001,B01111,B00000 }; // å
  byte upperAe[8]     = { B01111,B10100,B10100,B11110,B10100,B10100,B10111,B00000 }; // Æ
  byte upperOe[8]     = { B00001,B01110,B10011,B10101,B11001,B01110,B10000,B00000 }; // Ø
  byte upperAa[8]     = { B00100,B00000,B01110,B10001,B11111,B10001,B10001,B00000 }; // Å

  lcd.createChar(0, hearthFull); 
  lcd.createChar(1, hearthEmpty); 
  lcd.createChar(2, lowerAe); 
  lcd.createChar(3, lowerOe); 
  lcd.createChar(4, lowerAa); 
  lcd.createChar(5, upperAe); 
  lcd.createChar(6, upperOe); 
  lcd.createChar(7, upperAa); 
  
  lcd.clear();
  for (int i = 0; i < 40; i++) {
    lcd.setCursor(0,0);
    lcd.print("Norwegian chars: ");
    lcd.print(char(i % 2));
    lcd.setCursor(0,2);
    lcd.print("      \2  \3  \4");
    lcd.setCursor(0,3);
    lcd.print("      \5  \6  \7");
    delay(300);
  }
}

void loop() {  
  loremIpsum();
  loremIpsumScrolling();
  norwegianLetters();
}

