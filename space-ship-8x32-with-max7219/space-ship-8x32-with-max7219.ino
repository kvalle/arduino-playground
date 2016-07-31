#include "LedControl.h"

/*
 * Config 
 */

// SPI connector pins
#define SPI_DIN  2
#define SPI_CLK  4
#define SPI_CS   3

// number of displays connected
#define DISPLAYS 4

// time to sleep in ms
#define FRAME 5

// enable for debug mode
#define DEBUG false

#define BUZZER_PIN 5
#define BUTTON_PIN 6


/*
 * Model
 */

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, DISPLAYS);


int shot = -1;
byte ship_position = 1;


/*
 * Update
 */

void update_bullet()
{
  if (shot < 0) {
    return;
  } else if (shot >= 8 * lc.getDeviceCount()) {
    shot = -1;
  } else {
    shot++;  
  }
}



/*
 * View
 */


void shoot() {
  shot = 0;
  tone(BUZZER_PIN, 2960, 50);
}


void debug(String msg) 
{
  if (DEBUG) {
    Serial.println(msg);
  }
}

void draw_ship() 
{
  lc.setLed(3, ship_position - 1, 0, true);
  lc.setLed(3, ship_position - 1, 1, true);
  lc.setLed(3, ship_position, 1, true);
  lc.setLed(3, ship_position, 2, true);
  lc.setLed(3, ship_position + 1, 0, true);
  lc.setLed(3, ship_position + 1, 1, true);
}

void draw_bullet()
{
  if (shot < 0) {
    return;
  }
  
  byte displays = lc.getDeviceCount() - 1;
  byte col = shot % 8;
  byte disp = displays - (shot / 8);

  for (int d = 0; d < displays; d++) {
    for (int c = 0; c < 8; c++) {
      lc.setLed(d, 4, c, c == col && d == disp);
    }
  }
}


void setup()
{  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // setup displays
  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 7);
    lc.clearDisplay(i);
  }

  if (DEBUG) {
    Serial.begin(9600);
  }
}

void loop()
{
  // input
  if (digitalRead(BUTTON_PIN) == LOW) {
    shoot();
    ship_position++;
  }

  // update
  update_bullet();

  // view
  draw_ship();
  draw_bullet();

  delay(FRAME);
}

