#define signalPin 8

void setup() {
  Serial.begin (9600);
  pinMode(signalPin, INPUT);
}

void loop() {
  int signal = digitalRead(signalPin);
  Serial.println(signal);
  delay(100);
}

