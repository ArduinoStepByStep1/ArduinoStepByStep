#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define TRIG_PIN 2
#define ECHO_PIN 3
#define RX_PIN 10
#define TX_PIN 11

SoftwareSerial mySerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini myDFPlayer;

const float THRESHOLD_CM = 5.0;
const int SAMPLES = 5;
const int REQUIRED_STABLE = 3;
const unsigned long ESTIMATED_MP3_MS = 3000;

int aboveCount = 0;
int belowCount = 0;
bool playedOnce = false;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

long medianDistance() {
  long values[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    values[i] = getDistance();
    delay(20);
  }

  for (int i = 0; i < SAMPLES - 1; i++) {
    for (int j = i + 1; j < SAMPLES; j++) {
      if (values[j] < values[i]) {
        long t = values[i];
        values[i] = values[j];
        values[j] = t;
      }
    }
  }

  return values[SAMPLES / 2];
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer not found");
    while (true);
  }

  myDFPlayer.volume(10);
  Serial.println("System ready");
}

void loop() {
  long distance = medianDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > THRESHOLD_CM) {
    aboveCount++;
    belowCount = 0;
  } else {
    belowCount++;
    aboveCount = 0;
  }

  // PLAY 3 TIMES
  if (!playedOnce && aboveCount >= REQUIRED_STABLE) {
    myDFPlayer.play(1);
    delay(ESTIMATED_MP3_MS + 200);

    myDFPlayer.play(2);
    delay(ESTIMATED_MP3_MS + 200);

    myDFPlayer.play(3);
    delay(ESTIMATED_MP3_MS + 200);

    playedOnce = true;
    aboveCount = 0;
  }

  // RESET
  if (playedOnce && belowCount >= REQUIRED_STABLE) {
    playedOnce = false;
    belowCount = 0;
  }

  delay(100);
}
