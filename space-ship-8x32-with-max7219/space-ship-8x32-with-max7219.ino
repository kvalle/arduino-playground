#include <Button.h>
#include "LedControl.h"

/*
   PEW PEW

   Your typical simple space ship side scrolling asteroid shooter.

   Currently hard coded to work with four 8x8 LED matrices used side
   by side as a 8x32 display, but could probably easily be reworked
   to work with other configurations.
*/

/*
   Config
*/

// SPI connector pins
#define SPI_DIN  2
#define SPI_CLK  4
#define SPI_CS   3

// display dimensions
#define NUMBER_OF_ROWS 8
#define NUMBER_OF_COLS 32

// button pins
#define BUZZER_PIN 5
#define FIRE_BUTTON_PIN 6
#define DOWN_BUTTON_PIN 7
#define UP_BUTTON_PIN 8

// time to sleep in ms
#define FRAME_MS 5

#define MAX_ASTEROIDS_PER_ROW 10

// enable for debug mode
#define DEBUG false


/*
   Game state
*/

// Used to keep track of game time, for things only changing every n-th frame
int frame = 0;

// Which row the ship is centered on. Shoudl be in range [1, 6] in order
// for whole ship to be shown.
byte ship_position = 3;

// Bullets are represnted by one long per row, each bit representing
// a bullet either present or absent in the corresponding column.
long bullets[NUMBER_OF_ROWS];

// List of asteroid positions.
// There are 8 rows x max 10 asteroids per row.
int asteroids[NUMBER_OF_ROWS][MAX_ASTEROIDS_PER_ROW];

// When to show game splash screen, and when to have fun :)
int game_running = false;

// Buffer used by the draw() method for updating the displays
long screen_buffer[8];

/*
   Inputs
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
    if (game_running) {
      debug("pew pew\n");
      shoot();
    } else {
      new_game();
    }
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
   Output
*/

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, 4);

void draw()
{
  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    for (int d = 0; d < lc.getDeviceCount(); d++) {
      lc.setRow(d, row, (screen_buffer[row] >> (d * 8)));
    }
  }
}

/*
   Update - shit that changes the game state
*/

void game_over()
{
  game_running = false;
  game_over_screen();
}

void new_game()
{
  frame = 0;
  ship_position = 3;

  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    // stop bullets from flying
    bullets[row] = 0L;

    for (int i = 0; i < MAX_ASTEROIDS_PER_ROW; i++) {
      // move all asteroids off screen
      asteroids[row][i] = NUMBER_OF_COLS;
    }
  }

  game_running = true;
}

void move_bullets()
{
  for (int i = 0; i < NUMBER_OF_ROWS; i++) {
    // take a step to the right
    bullets[i] >>= 1;
  }
}

void move_asteroids()
{
  if (frame % 8 != 0) {
    return;
  }

  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    for (int i = 0; i < MAX_ASTEROIDS_PER_ROW; i++) {
      if (asteroids[row][i] < NUMBER_OF_COLS) {
        asteroids[row][i]++;
      }
    }
  }
}

void spawn_asteroids()
{
  if (frame % 13 != 0) {
    return;
  }

  new_asteroid(random(1, 7));
}

void new_asteroid(int row)
{
  for (int i = 0; i < MAX_ASTEROIDS_PER_ROW; i++) {
    if (asteroids[row][i] >= NUMBER_OF_COLS) {
      asteroids[row][i] = 0;
      return;
    }
  }
}

void detect_collisions()
{
  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    for (int i = 0; i < MAX_ASTEROIDS_PER_ROW; i++) {
      int pos = asteroids[row][i];

      // detect shoting of asteroids
      long bullet_mask = bullets[row];
      if (bullet_mask & (1L << pos)) {
        debug("bang\n");
        asteroids[row][i] = NUMBER_OF_ROWS; // remove asteroid
        bullets[row] &= ~(1L << pos); // remove bullet
      }

      // detect ship collisions
      unsigned long ship_mask = get_ship_mask(row);
      if (ship_mask & (1L << pos)) {
        game_over();
      }
    }
  }
}

void shoot()
{
  // new bullets start at column 2, to not overlap with the space ship
  bullets[ship_position] |= (1L << (NUMBER_OF_COLS - 3));

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

void update_screen_buffer()
{
  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    long bullets_mask = bullets[row];
    long ship_mask = get_ship_mask(row);
    long asteroid_mask = get_asteroid_mask(row);

    screen_buffer[row] = bullets_mask | ship_mask | asteroid_mask;
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

  for (int i = 0; i < MAX_ASTEROIDS_PER_ROW; i++) {
    if (asteroids[row][i] != NUMBER_OF_COLS) {
      mask |= (1L << asteroids[row][i]);
    }
  }

  return mask;
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
  for (int d = 0; d < lc.getDeviceCount(); d++) {
    lc.shutdown(d, false);
    lc.setIntensity(d, 7);
    lc.clearDisplay(d);
  }

  if (DEBUG) {
    Serial.begin(9600);
  }
}

void advance_game()
{
  // update
  detect_collisions();
  move_asteroids();
  detect_collisions();
  move_bullets();
  spawn_asteroids();
  update_screen_buffer();

  // view
  draw();

  // wait
  step_frame();
}

void game_over_screen()
{
  long game[8] = {
    0B00000000000000000000000000000000L,
    0B00000111000011000010001000111000L,
    0B00001000100100100101010100100000L,
    0B00001000000100100100100100100000L,
    0B00001001100111100100100100111000L,
    0B00001000100100100100100100100000L,
    0B00000111000100100100000100111000L,
    0B00000000000000000000000000000000L
  };

  for (int r = 0; r < 8; r++) {
    long mask = game[r];

    for (int d = 0; d < 4; d++) {
      lc.setRow(d, r, (mask >> (d * 8)));
    }
  }
  delay(1000);

  long over[8] = {
    0B00000000000000000000000000000000L,
    0B00000111000100001001110011100000L,
    0B00001000100100001001000010010000L,
    0B00001000100100001001000010010000L,
    0B00001000100100001001110011100000L,
    0B00001000100010010001000010010000L,
    0B00000111000001100001110010010000L,
    0B00000000000000000000000000000000L
  };

  for (int r = 0; r < 8; r++) {
    long mask = over[r];

    for (int d = 0; d < 4; d++) {
      lc.setRow(d, r, (mask >> (d * 8)));
    }
  }
  delay(1000);

  for (int d = 0; d < 4; d++) {
    lc.clearDisplay(d);
  }
  delay(500);
}

void pre_game_screen()
{
  long rows[8] = {
    0B00000000000000000000000000000000,
    0B00000000000000000000000000000000,
    0B00111011101000100111011101000100,
    0B00101010001000100101010001000100,
    0B00111011101010100111011101010100,
    0B00100010001101100100010001101100,
    0B00100011101000100100011101000100,
    0B00000000000000000000000000000000
  };

  for (int r = 0; r < 8; r++) {
    long mask = rows[r];

    for (int d = 0; d < 4; d++) {
      lc.setRow(d, r, (mask >> (d * 8)));
    }
  }
}

void loop()
{
  check_buttons();

  if (!game_running) {
    pre_game_screen();
  } else {
    advance_game();
  }
}

