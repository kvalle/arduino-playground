int pin1 = 1;
int pin2 = 3;
int pin3 = 5;

int led = 13;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);

  digitalWrite(led, HIGH);
  analogWrite(pin1, 1023);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  delay(10000);

  analogWrite(pin1, 300);
  delay(5);
    
  analogWrite(pin1, 0);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);

}

void loop() {}

