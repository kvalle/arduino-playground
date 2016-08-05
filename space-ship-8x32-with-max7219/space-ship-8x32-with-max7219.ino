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
#define MATRIX_SPI_DIN    2
#define MATRIX_SPI_CLK    4
#define MATRIX_SPI_CS     3
#define SEGMENTS_SPI_DIN 12
#define SEGMENTS_SPI_CLK 11
#define SEGMENTS_SPI_CS  10

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
#define DEBUG true


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

long score = 0;

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

LedControl lc_matrix = LedControl(MATRIX_SPI_DIN, MATRIX_SPI_CLK, MATRIX_SPI_CS, 4);
LedControl lc_segments = LedControl(SEGMENTS_SPI_DIN, SEGMENTS_SPI_CLK, SEGMENTS_SPI_CS, 1);

void draw()
{
  for (int row = 0; row < NUMBER_OF_ROWS; row++) {
    for (int d = 0; d < lc_matrix.getDeviceCount(); d++) {
      lc_matrix.setRow(d, row, (screen_buffer[row] >> (d * 8)));
    }
  }

  debug("score is " + String(score) + "\n");
  set_score(score);
}

void clearDisplays()
{
  for (int d = 0; d < lc_matrix.getDeviceCount(); d++) {
    lc_matrix.clearDisplay(d);
  }
}

void initDisplays()
{
  for (int d = 0; d < lc_matrix.getDeviceCount(); d++) {
    lc_matrix.shutdown(d, false);
    lc_matrix.setIntensity(d, 7);
    lc_matrix.clearDisplay(d);
  }

  lc_segments.shutdown(0, false);
  lc_segments.setIntensity(0, 5);
  lc_segments.clearDisplay(0);
}

void set_score(long num)
{
  if (num > 99999999L) {
    num = 99999999L;
  }

  byte digit = 0;
  while (true) {
    Serial.println(num % 10);
    lc_segments.setDigit(0, digit++, num % 10, false);
    num /= 10;
    if (num <= 0) break;
  }
}


/*
   Update - shit that changes the game state
*/

void game_over()
{
  debug("game over");
  game_running = false;
  game_over_screen();
}

void new_game()
{
  debug("new game");
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
        asteroids[row][i] = NUMBER_OF_COLS; // remove asteroid
        bullets[row] &= ~(1L << pos); // remove bullet
        score++;
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
  initDisplays();

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
  memcpy(screen_buffer, game, sizeof(game));
  draw();
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
  memcpy(screen_buffer, over, sizeof(over));
  draw();
  delay(1000);

  clearDisplays();
  delay(500);
}

void pre_game_screen()
{
  long pewpew[] = {
    0B00000000000000000000000000000000,
    0B00000000000000000000000000000000,
    0B00111011101000100111011101000100,
    0B00101010001000100101010001000100,
    0B00111011101010100111011101010100,
    0B00100010001101100100010001101100,
    0B00100011101000100100011101000100,
    0B00000000000000000000000000000000
  };
  memcpy(screen_buffer, pewpew, sizeof(pewpew));
  draw();
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

