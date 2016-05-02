// ca 2000: 220?
// kl 0015: 360 og faller...?

int wait = 10000;

int red = 8;
int green = 9;
int blue = 10;

int digital = 2;
int analog = A0;

int limit_too_much = 200;
int limit_too_little = 500;

void setup() {
  pinMode(2,  INPUT);  // D0
  pinMode(A0, INPUT); // A0

  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);

  Serial.begin(9600);
}

void rgb(int r, int g, int b) {
  digitalWrite(red, r);
  digitalWrite(green, g);
  digitalWrite(blue, b);
}

void loop() {
  int analog = analogRead(A0);
  Serial.println(analog);
  
  if (analog < limit_too_much) {
    // [0,199] -> too much water, blue light.
    rgb(LOW, LOW, HIGH);
  } else if (analog < limit_too_little) {
    // [200, 499] -> water si good, green light.
    rgb(LOW, HIGH, LOW);
  } else {
    // [500,1023] -> too little water, red light.
    rgb(HIGH, LOW, LOW);
  }

  delay(wait);
}

