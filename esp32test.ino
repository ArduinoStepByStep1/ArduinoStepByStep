void setup() {
  pinMode(2, OUTPUT); // oder LED_BUILTIN, je nach Board
}

void loop() {
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
}

