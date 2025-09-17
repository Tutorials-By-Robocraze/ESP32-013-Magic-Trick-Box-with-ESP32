#include <ESP32Servo.h>

Servo lockServo;

int buttonPin = 15;
int redLED = 25;
int greenLED = 26;
int servoPin = 13;

// Button variables
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

int pressCount = 0;
const int requiredPresses = 2;

// Servo movement
int servoPos = 0;       // Current servo position
int servoTarget = 0;    // Target servo position
const int servoStepDelay = 15; // ms between servo steps
unsigned long lastServoMoveTime = 0;

// Unlock timing
bool isUnlocked = false;
unsigned long unlockStartTime = 0;
const unsigned long unlockDuration = 6000; // 6 seconds

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  lockServo.attach(servoPin);
  lockServo.write(0);
  servoPos = 0;

  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);

  Serial.println("Ready. Press button 2 times to unlock.");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Button reading ---
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    if (lastButtonState == HIGH && reading == LOW) {
      pressCount++;
      Serial.print("Button pressed! Count: ");
      Serial.println(pressCount);

      if (pressCount >= requiredPresses && !isUnlocked) {
        isUnlocked = true;
        servoTarget = 90;          // Unlock
        unlockStartTime = currentMillis;
        digitalWrite(redLED, LOW);
        digitalWrite(greenLED, HIGH);
        Serial.println("Unlocking...");
      }
    }
  }

  lastButtonState = reading;

  // --- Servo movement (non-blocking) ---
  if (currentMillis - lastServoMoveTime >= servoStepDelay) {
    lastServoMoveTime = currentMillis;

    if (servoPos < servoTarget) {
      servoPos++;
      lockServo.write(servoPos);
    } else if (servoPos > servoTarget) {
      servoPos--;
      lockServo.write(servoPos);
    }
  }

  // --- Lock back after unlock duration ---
  if (isUnlocked && (currentMillis - unlockStartTime >= unlockDuration)) {
    isUnlocked = false;
    servoTarget = 0;            // Lock
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    pressCount = 0;             // Reset for next round
    Serial.println("Locking...");
  }
}
