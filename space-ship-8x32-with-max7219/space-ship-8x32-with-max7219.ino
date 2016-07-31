#include <Button.h>
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
#define DEBUG true

// button pins used
#define BUZZER_PIN 5
#define FIRE_BUTTON_PIN 6
#define UP_BUTTON_PIN 7
#define DOWN_BUTTON_PIN 8



/*
 * State
 */

LedControl lc = LedControl(SPI_DIN, SPI_CLK, SPI_CS, DISPLAYS);

int shot = -1;
byte ship_position = 1;

Button fireButton = Button(FIRE_BUTTON_PIN, &fireCallback);
Button upButton = Button(UP_BUTTON_PIN, &upCallback);
Button downButton = Button(DOWN_BUTTON_PIN, &downCallback);


/*
 * Input
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
    debug("pew pew");
    shoot();
  }
}

void upCallback(Button* button)
{ 
  if (button->pressed && ship_position <= 5){
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

void shoot() {
  shot = 0;
  tone(BUZZER_PIN, 2960, 50);
}



/*
 * View
 */

void debug(String msg) 
{
  if (DEBUG) {
    Serial.println(msg);
  }
}

void draw_ship() 
{
  lc.setColumn(3, 0, B00000000);
  lc.setColumn(3, 1, B00000000);
  lc.setColumn(3, 2, B00000000);
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
  pinMode(FIRE_BUTTON_PIN, INPUT_PULLUP);
  
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
  for (int i = 0; i < buttonAmount; i++){
    buttons[i]->read();
  }

  // update
  update_bullet();

  // view
  draw_ship();
  draw_bullet();

  delay(FRAME);
}

