#include <Button.h>
#include "LedControl.h"

/*
   Config
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

// button pins used
#define BUZZER_PIN 5
#define FIRE_BUTTON_PIN 6
#define UP_BUTTON_PIN 7
#define DOWN_BUTTON_PIN 8


/*
   State
*/

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, DISPLAYS);

byte ship_position = 1;

Button fireButton = Button(FIRE_BUTTON_PIN, &fireCallback);
Button upButton = Button(UP_BUTTON_PIN, &upCallback);
Button downButton = Button(DOWN_BUTTON_PIN, &downCallback);

unsigned long bullets[8] = { 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L };

/*
   Input
*/

const int buttonAmount = 3;
Button* buttons[buttonAmount] = {
  &fireButton,
  &upButton,
  &downButton
};

void fireCallback(Button* button)
{
  if (button->pressed) {
    debug("pew pew\n");
    shoot();
  }
}

void upCallback(Button* button)
{
  if (button->pressed && ship_position <= 5) {
    ship_position++;
  }
}

void downCallback(Button* button)
{
  if (button->pressed && ship_position >= 2) {
    ship_position--;
  }
}


/*
   Update
*/

void update_bullet()
{
  for (int i = 0; i < 8; i++) {
    unsigned long row = bullets[i];  
    bullets[i] = row >> 1;
  }
}

void shoot() {
  unsigned long row = bullets[ship_position];
  bullets[ship_position] = row | 0B00010000000000000000000000000000L;

  tone(BUZZER_PIN, 2960, 50);
}


/*
   View
*/

void debug(String msg)
{
  if (DEBUG) {
    Serial.print(msg);
  }
}

void draw_ship()
{
  byte offset = 6 - ship_position;

  lc.setColumn(3, 0, B101 << offset);
  lc.setColumn(3, 1, B111 << offset);
  lc.setColumn(3, 2, B010 << offset);
}

void draw_bullet()
{
  for (int r = 0; r < 8; r++) {
    unsigned long row = bullets[r];
    for (int d = 0; d < 3; d++) {
      lc.setRow(d, r, row >> (d * 8));
    }
  }
  
}


void setup()
{
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
  for (int i = 0; i < buttonAmount; i++) {
    buttons[i]->read();
  }

  // update
  update_bullet();

  // view
  draw_bullet();
  draw_ship();

  delay(FRAME);
}

