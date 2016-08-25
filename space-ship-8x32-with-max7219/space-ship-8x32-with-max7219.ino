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
#define BUZZER_PIN2 9
#define FIRE_BUTTON_PIN 6
#define DOWN_BUTTON_PIN 7
#define UP_BUTTON_PIN 8

// time to sleep in ms
#define FRAME_MS 5

#define MAX_ASTEROIDS_PER_ROW 10

#define DEBUG false
#define MUSIC_ENABLED false


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
   Music stuff
*/


static const byte SILENT = 0;
static const byte NOTE_C0 = 1;
static const byte NOTE_DB0 = 2;
static const byte NOTE_CS0 = 3;
static const byte NOTE_D0 = 4;
static const byte NOTE_EB0 = 5;
static const byte NOTE_DS0 = 6;
static const byte NOTE_E0 = 7;
static const byte NOTE_F0 = 8;
static const byte NOTE_GB0 = 9;
static const byte NOTE_FS0 = 10;
static const byte NOTE_G0 = 11;
static const byte NOTE_AB0 = 12;
static const byte NOTE_GS0 = 13;
static const byte NOTE_A0 = 14;
static const byte NOTE_BB0 = 15;
static const byte NOTE_AS0 = 16;
static const byte NOTE_B0 = 17;
static const byte NOTE_C1 = 18;
static const byte NOTE_DB1 = 19;
static const byte NOTE_CS1 = 20;
static const byte NOTE_D1 = 21;
static const byte NOTE_EB1 = 22;
static const byte NOTE_DS1 = 23;
static const byte NOTE_E1 = 24;
static const byte NOTE_F1 = 25;
static const byte NOTE_GB1 = 26;
static const byte NOTE_FS1 = 27;
static const byte NOTE_G1 = 28;
static const byte NOTE_AB1 = 29;
static const byte NOTE_GS1 = 30;
static const byte NOTE_A1 = 31;
static const byte NOTE_BB1 = 32;
static const byte NOTE_AS1 = 33;
static const byte NOTE_B1 = 34;
static const byte NOTE_C2 = 35;
static const byte NOTE_DB2 = 36;
static const byte NOTE_CS2 = 37;
static const byte NOTE_D2 = 38;
static const byte NOTE_EB2 = 39;
static const byte NOTE_DS2 = 40;
static const byte NOTE_E2 = 41;
static const byte NOTE_F2 = 42;
static const byte NOTE_GB2 = 43;
static const byte NOTE_FS2 = 44;
static const byte NOTE_G2 = 45;
static const byte NOTE_AB2 = 46;
static const byte NOTE_GS2 = 47;
static const byte NOTE_A2 = 48;
static const byte NOTE_BB2 = 49;
static const byte NOTE_AS2 = 50;
static const byte NOTE_B2 = 51;
static const byte NOTE_C3 = 52;
static const byte NOTE_DB3 = 53;
static const byte NOTE_CS3 = 54;
static const byte NOTE_D3 = 55;
static const byte NOTE_EB3 = 56;
static const byte NOTE_DS3 = 57;
static const byte NOTE_E3 = 58;
static const byte NOTE_F3 = 59;
static const byte NOTE_GB3 = 60;
static const byte NOTE_FS3 = 61;
static const byte NOTE_G3 = 62;
static const byte NOTE_AB3 = 63;
static const byte NOTE_GS3 = 64;
static const byte NOTE_A3 = 65;
static const byte NOTE_BB3 = 66;
static const byte NOTE_AS3 = 67;
static const byte NOTE_B3 = 68;
static const byte NOTE_C4 = 69;
static const byte NOTE_DB4 = 70;
static const byte NOTE_CS4 = 71;
static const byte NOTE_D4 = 72;
static const byte NOTE_EB4 = 73;
static const byte NOTE_DS4 = 74;
static const byte NOTE_E4 = 75;
static const byte NOTE_F4 = 76;
static const byte NOTE_GB4 = 77;
static const byte NOTE_FS4 = 78;
static const byte NOTE_G4 = 79;
static const byte NOTE_AB4 = 80;
static const byte NOTE_GS4 = 81;
static const byte NOTE_A4 = 82;
static const byte NOTE_BB4 = 83;
static const byte NOTE_AS4 = 84;
static const byte NOTE_B4 = 85;
static const byte NOTE_C5 = 86;
static const byte NOTE_DB5 = 87;
static const byte NOTE_CS5 = 88;
static const byte NOTE_D5 = 89;
static const byte NOTE_EB5 = 90;
static const byte NOTE_DS5 = 91;
static const byte NOTE_E5 = 92;
static const byte NOTE_F5 = 93;
static const byte NOTE_GB5 = 94;
static const byte NOTE_FS5 = 95;
static const byte NOTE_G5 = 96;
static const byte NOTE_AB5 = 97;
static const byte NOTE_GS5 = 98;
static const byte NOTE_A5 = 99;
static const byte NOTE_BB5 = 100;
static const byte NOTE_AS5 = 101;
static const byte NOTE_B5 = 102;
static const byte NOTE_C6 = 103;
static const byte NOTE_DB6 = 104;
static const byte NOTE_CS6 = 105;
static const byte NOTE_D6 = 106;
static const byte NOTE_EB6 = 107;
static const byte NOTE_DS6 = 108;
static const byte NOTE_E6 = 109;
static const byte NOTE_F6 = 110;
static const byte NOTE_GB6 = 111;
static const byte NOTE_FS6 = 112;
static const byte NOTE_G6 = 113;
static const byte NOTE_AB6 = 114;
static const byte NOTE_GS6 = 115;
static const byte NOTE_A6 = 116;
static const byte NOTE_BB6 = 117;
static const byte NOTE_AS6 = 118;
static const byte NOTE_B6 = 119;
static const byte NOTE_C7 = 120;
static const byte NOTE_DB7 = 121;
static const byte NOTE_CS7 = 122;
static const byte NOTE_D7 = 123;
static const byte NOTE_EB7 = 124;
static const byte NOTE_DS7 = 125;
static const byte NOTE_E7 = 126;
static const byte NOTE_F7 = 127;
static const byte NOTE_GB7 = 128;
static const byte NOTE_FS7 = 129;
static const byte NOTE_G7 = 130;
static const byte NOTE_AB7 = 131;
static const byte NOTE_GS7 = 132;
static const byte NOTE_A7 = 133;
static const byte NOTE_BB7 = 134;
static const byte NOTE_AS7 = 135;
static const byte NOTE_B7 = 136;
static const byte NOTE_C8 = 137;
static const byte NOTE_DB8 = 138;
static const byte NOTE_CS8 = 139;
static const byte NOTE_D8 = 140;
static const byte NOTE_EB8 = 141;
static const byte NOTE_DS8 = 142;
static const byte NOTE_E8 = 143;
static const byte NOTE_F8 = 144;
static const byte NOTE_GB8 = 145;
static const byte NOTE_FS8 = 146;
static const byte NOTE_G8 = 147;
static const byte NOTE_AB8 = 148;
static const byte NOTE_GS8 = 149;
static const byte NOTE_A8 = 150;
static const byte NOTE_BB8 = 151;
static const byte NOTE_AS8 = 152;
static const byte NOTE_B8 = 153;

static const byte WHOLE = 0;
static const byte HALF = 1;
static const byte QUARTER = 2;
static const byte EIGHTH = 3;
static const byte SIXTH = 4;
static const byte THIRD = 5;

double note_durations[] = { 1.0, 0.5, 0.25, 0.125, 0.1667, 0.3333 };

int note_frequencies[] = {
  0, 16, 17, 17, 18, 19, 19, 21, 22, 23, 23, 245,
  26, 26, 28, 29, 29, 31, 33, 35, 35, 37, 39, 39,
  41, 44, 46, 46, 49, 52, 52, 55, 58, 58, 62, 65,
  69, 69, 73, 78, 78, 82, 87, 93, 93, 98, 104, 104,
  110, 117, 117, 123, 131, 139, 139, 147, 156, 156,
  165, 175, 185, 185, 196, 208, 208, 220, 233, 233,
  247, 262, 277, 277, 294, 311, 311, 330, 349, 370,
  370, 392, 415, 415, 440, 466, 466, 494, 523, 554,
  554, 587, 622, 622, 659, 698, 740, 740, 784, 831,
  831, 880, 932, 932, 988, 1047, 1109, 1109, 1175,
  1245, 1245, 1319, 1397, 1480, 1480, 1568, 1661,
  1661, 1760, 1865, 1865, 1976, 2093, 2217, 2217,
  2349, 2489, 2489, 2637, 2794, 2960, 2960, 3136,
  3322, 3322, 3520, 3729, 3729, 3951, 4186, 4435,
  4435, 4699, 4978, 4978, 5274, 5588, 5920, 5920,
  6272, 6645, 6645, 7040, 7459, 7459, 7902
};

const byte notes[] = {
  // comments correspond to page/line in sheet at
  // http://www.mariopiano.com/mario-sheet-music-overworld-main-theme.html

  // 1/1
  NOTE_E7, NOTE_E7, SILENT, NOTE_E7,
  SILENT, NOTE_C7, NOTE_E7, SILENT,
  NOTE_G7, SILENT, SILENT,
  NOTE_G6, SILENT, SILENT,

  // 1/2
  NOTE_C7, SILENT, SILENT, NOTE_G6,
  SILENT, NOTE_E6, SILENT,
  SILENT, NOTE_A6, SILENT, NOTE_B6,
  SILENT, NOTE_BB6, NOTE_A6, SILENT,

  // 1/3
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, SILENT, NOTE_F7, NOTE_G7,
  SILENT, NOTE_E7, SILENT, NOTE_C7,
  NOTE_D7, NOTE_B6, SILENT, SILENT,

  // 1/4 (same as 1/2)
  NOTE_C7, SILENT, SILENT, NOTE_G6,
  SILENT, NOTE_E6, SILENT,
  SILENT, NOTE_A6, SILENT, NOTE_B6,
  SILENT, NOTE_BB6, NOTE_A6, SILENT,

  // 1/5
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, SILENT, NOTE_F7, NOTE_G7,
  SILENT, NOTE_E7, SILENT, NOTE_C7,
  NOTE_D7, NOTE_B6, SILENT
};

const byte durations[] = {

  // 1/1
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, HALF,
  QUARTER, QUARTER, HALF,

  // 1/2
  QUARTER, QUARTER, QUARTER, QUARTER,
  HALF, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,

  // 1/3
  THIRD,   THIRD,   THIRD,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,

  // 1/4
  QUARTER, QUARTER, QUARTER, QUARTER,
  HALF, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,

  // 1/5
  THIRD,   THIRD,   THIRD,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, QUARTER, QUARTER,
  QUARTER, QUARTER, HALF
};

void sound_game_over()
{
  tone(BUZZER_PIN, note_frequencies[NOTE_G4], 600);
}

void sound_pew()
{
  tone(BUZZER_PIN, note_frequencies[NOTE_G7], 50);
}

int index = 0;
unsigned long previousMillis = 0;

int numberOfNotes = sizeof(notes) / sizeof(notes[0]);
double const BPM = 320.0; // beats per minute (quarter notes)
double const PAUSE = 0.2; // portion of note should be silent before next note
int whole_duration = (60.0f / BPM) * 4 * 1000;

void play_melody()
{
  if (!MUSIC_ENABLED)
    return;
  
  unsigned long currentMillis = millis();

  if (index >= numberOfNotes) {
    index = 0;
  }

  double duration = whole_duration * note_durations[durations[index]];

  if (currentMillis >= (previousMillis + duration)) {
    previousMillis = currentMillis;
    index++;
    
    int freq = note_frequencies[notes[index]];
    int playDuration = duration * (1 - PAUSE);

    tone(BUZZER_PIN2, freq, playDuration);
  }
}

void initSound()
{
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUZZER_PIN2, OUTPUT);
}


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
    lc_matrix.setIntensity(d, 8);
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
  debug("game over\n");
  game_running = false;
  game_over_screen();
}

void new_game()
{
  debug("new game\n");
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
        debug("[explosion]\n");
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

  sound_pew();
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
  frame %= 100000;
  delay(FRAME_MS);
}

void setup()
{
  initDisplays();
  initSound();

  if (DEBUG) {
    Serial.begin(9600);
  }

  debug("started\n");
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
}

void game_over_screen()
{
  sound_game_over();

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
  play_melody();

  if (!game_running) {
    pre_game_screen();
  } else {
    advance_game();
  }

  step_frame();
}

