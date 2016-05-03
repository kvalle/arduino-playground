/* interrupt routine for Rotary Encoders


   The average rotary encoder has three pins, seen from front: A C B
   Clockwise rotation A(on)->B(on)->A(off)->B(off)
   CounterCW rotation B(on)->A(on)->B(off)->A(off)

   and may be a push switch with another two pins, pulled low at pin 8 in this case
  
*/

const int RED = 6;
const int GREEN = 9;
const int BLUE = 10;

// usually the rotary encoders three pins have the ground pin in the middle
enum PinAssignments {
  encoderPinA = 2,   // right (labeled DT on our decoder, yellow wire)
  encoderPinB = 3,   // left (labeled CLK on our decoder, green wire)
  clearButton = 11   // switch (labeled SW on our decoder, orange wire)
};

volatile byte encoderPos = 0;     // a counter for the dial
byte lastReportedPos = 1;         // change management
static boolean rotating = false;  // debounce management

// interrupt service routine vars
boolean A_set = false;              
boolean B_set = false;

// color value
int color_rgb[] = {254, 254, 254};
int color_index = 0;

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP); 
  pinMode(clearButton, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);

  Serial.begin(9600);  // output
}

void loop() { 
  rotating = true;  // reset the debouncer

  if (lastReportedPos != encoderPos) {
    Serial.print("Position: ");
    Serial.println(encoderPos);
      
    lastReportedPos = encoderPos;
    color(encoderPos);
  }

  if (digitalRead(clearButton) == LOW )  {
    color_index += 1;
    if (color_index > 2) {
      color_index = 0;
    }
    Serial.print("Index is now ");
    Serial.println(color_index);
    delay(250); // debounce
  }
}

void color(byte val) {
  // map from linear to exponential
  int color_val = val; // fscale(0, 255, 0, 255, 255 - val, 2);
  color_rgb[color_index] = color_val;

  Serial.print("Color: ");
  Serial.println(color_val);

  //printArray(color_rgb, 3);
  
  rgb(color_rgb);
}

void rgb(int color_rgb[]) {
  analogWrite(RED, color_rgb[0]);
  analogWrite(GREEN, color_rgb[1]);
  analogWrite(BLUE, color_rgb[2]);
}

void printArray(int array[], int len) {
  //int i;
  //for (i = 0; i < len; i = i + 1) {
  //  Serial.print(array[i]);
  //  Serial.print(" ");
  //}
  //Serial.println();
}


// Interrupt on A changing state
void doEncoderA() {
  if (rotating) {
    delay (1);
  }

  // Test transition, did things really change? 
  if( digitalRead(encoderPinA) != A_set ) {  // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set ) {
      encoderPos += 1;
    }

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if (rotating) {
    delay (1);
  }
  
  if( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;
    
    //  adjust counter - 1 if B leads A
    if( B_set && !A_set ) {
      encoderPos -= 1;
    }
    
    rotating = false;
  }
}


