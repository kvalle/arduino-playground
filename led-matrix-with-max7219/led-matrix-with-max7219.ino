#include "LedControl.h"

// SPI connector pins
#define SPI_DIN  2
#define SPI_CLK  4
#define SPI_CS   3

// number of displays connected
#define DISPLAYS 4

// time to sleep in ms
#define FRAME 50

// enable for debug mode
#define DEBUG true

#define BUZZER_PIN 5
#define BUTTON_PIN 6

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, DISPLAYS);

int shot = -1;

byte ship[] =
{
  B00000000,
  B11110000,
  B01000000,
  B11111100,
  B11110111,
  B11111100,
  B01000000,
  B11110000
};


void setup()
{  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // setup displays
  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 7);
    lc.clearDisplay(i);
  }

  // draw ship
  for (int i = 0; i < 8; i++) {
    lc.setRow(3, i, ship[i]);
  }

  // shots fired!
  shot = 0;

  if (DEBUG) {
    Serial.begin(9600);
  }
}

void debug(String msg) 
{
  if (DEBUG) {
    Serial.println(msg);
  }
}

void shoot()
{
  byte displays = lc.getDeviceCount() - 1;
  
  byte col = shot % 8;
  byte disp = displays - (shot / 8);

  for (int d = 0; d < displays; d++) {
    for (int c = 0; c < 8; c++) {
      lc.setLed(d, 4, c, c == col && d == disp);
    }
  }

  if (shot >= 8 * lc.getDeviceCount()) {
    shot = -1;
    tone(BUZZER_PIN, 2960, 50);
  } else {
    shot++;  
  }
  
  delay(FRAME);
}

void loop()
{
  if (shot >= 0) {
    shoot();
  }
}

