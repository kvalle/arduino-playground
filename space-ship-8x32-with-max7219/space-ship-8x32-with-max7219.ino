#include <Button.h>
#include "LedControl.h"

/*
   Config
*/

// SPI connector pins
#define SPI_DIN  2
#define SPI_CLK  4
#define SPI_CS   3

// button pins
#define BUZZER_PIN 5
#define FIRE_BUTTON_PIN 6
#define DOWN_BUTTON_PIN 7
#define UP_BUTTON_PIN 8

// number of displays connected
#define DISPLAYS 4

// time to sleep in ms
#define FRAME_MS 5

// enable for debug mode
#define DEBUG true



/*
   State
*/

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, DISPLAYS);

byte ship_position = 3;
long bullets[8] = { 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L };

int asteroids[8][10] = {
  {3, 12, 100, 100, 100, 100, 100, 100, 100, 100},
  {9, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  {2, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  {21, 24, 100, 100, 100, 100, 100, 100, 100, 100},
  {7, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
  {0, 4, 15, 100, 100, 100, 100, 100, 100, 100}
};


/*
   Input
*/

Button fireButton = Button(FIRE_BUTTON_PIN, &fireCallback);
Button upButton = Button(UP_BUTTON_PIN, &upCallback);
Button downButton = Button(DOWN_BUTTON_PIN, &downCallback);

void check_buttons() {
  fireButton.read();
  upButton.read();
  downButton.read();
}

void fireCallback(Button* button)
{
  if (button->pressed) {
    debug("pew pew\n");
    shoot();
  }
}

void upCallback(Button* button)
{
  if (button->pressed) {
    flyUp();
  }
}

void downCallback(Button* button)
{
  if (button->pressed) {
    flyDown();
  }
}


/*
   Update
*/

void update_bullets()
{
  for (int i = 0; i < 8; i++) {
    // take a step to the right
    bullets[i] >>= 1;
  }
}

void shoot() {
  // new bullets start at index 2
  bullets[ship_position] |= (1L << 29);

  // pew pew
  tone(BUZZER_PIN, 2960, 50);
}

void flyDown() {
  if (ship_position < 6) {
    ship_position++;
  }
}

void flyUp() {
  if (ship_position > 1) {
    ship_position--;
  }
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

long get_ship_mask(int row)
{
  if (row == (ship_position - 1) || row == (ship_position + 1))
    return 0B110L << 29;
  else if (row == ship_position)
    return 0B011L << 29;
  else
    return 0;
}

long get_asteroid_mask(int row)
{
  long mask = 0L;
 
  for (int i = 0; i < 10; i++) {
    if (asteroids[row][i] != 100) {
      mask |= (1L << asteroids[row][i]);
    }
  }

  return mask;
}

void draw()
{
  for (int r = 0; r < 8; r++) {
    long bullets_mask = bullets[r];
    long ship_mask = get_ship_mask(r);
    long asteroid_mask = get_asteroid_mask(r);

    long mask = bullets_mask | ship_mask | asteroid_mask;

    for (int d = 0; d < 4; d++) {
      lc.setRow(d, r, (mask >> (d * 8)));
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
  check_buttons();

  // update
  update_bullets();

  // view
  draw();

  delay(FRAME_MS);
}

