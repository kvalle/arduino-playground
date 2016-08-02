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

int frame = 0;

byte ship_position = 3;
long bullets[8] = { 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L };

int asteroids[8][10] = {
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
  {32, 32, 32, 32, 32, 32, 32, 32, 32, 32}
};


/*
   Input
*/

Button fireButton = Button(FIRE_BUTTON_PIN, &fireCallback);
Button upButton = Button(UP_BUTTON_PIN, &upCallback);
Button downButton = Button(DOWN_BUTTON_PIN, &downCallback);

void check_buttons()
{
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

void move_bullets()
{
  for (int i = 0; i < 8; i++) {
    // take a step to the right
    bullets[i] >>= 1;
  }
}

void move_asteroids()
{
  if (frame % 20 != 0) {
    return;
  }

  for (int row = 0; row < 8; row++) {
    for (int i = 0; i < 10; i++) {
      if (asteroids[row][i] < 32) {
        asteroids[row][i]++;
      }
    }
  }
}

void spawn_asteroids()
{
  if (frame % 30 != 0) {
    return;
  }

  new_asteroid(random(1, 7));
}

void new_asteroid(int row)
{
  for (int i = 0; i < 10; i++) {
    if (asteroids[row][i] == 32) {
      asteroids[row][i] = 0;
      return;
    }
  }
}

void detect_collisions() {
  for (int row = 0; row < 8; row++) {
    for (int i = 0; i < 10; i++) {
      int pos = asteroids[row][i];
      long bullet_mask = bullets[row];
      if (bullet_mask & (1L << pos)) {
        debug("bang\n");
        asteroids[row][i] = 32;
      }
    }
  }
}

void shoot()
{
  // new bullets start at index 2
  bullets[ship_position] |= (1L << 29);

  // pew pew
  tone(BUZZER_PIN, 2960, 50);
}

void flyDown()
{
  if (ship_position < 6) {
    ship_position++;
  }
}

void flyUp()
{
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
    if (asteroids[row][i] != 32) {
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

void step_frame()
{
  frame++;
  frame %= 1000;
  delay(FRAME_MS);
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
  detect_collisions();
  spawn_asteroids();
  move_asteroids();
  move_bullets();

  // view
  draw();

  // wait
  step_frame();
}

