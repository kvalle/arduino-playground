#define trigPin 8
#define echoPin 9
#define buzzerPin 5

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance > 5 && distance < 10) {
    tone(buzzerPin, 988, 50);
    Serial.println("Beep!");
  }

  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}

