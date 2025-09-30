#include <Servo.h>

#define trigPin1 2
#define echoPin1 3
#define trigPin2 4
#define echoPin2 5
#define servoPin 6
#define triggerESP 7

Servo myServo;

// State tracking variables
bool slot1_occupied = false;
bool slot2_occupied = false;
bool slot1_scanned = false;
bool slot2_scanned = false;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(triggerESP, OUTPUT);
  digitalWrite(triggerESP, LOW);

  myServo.attach(servoPin);
  myServo.write(90); // Neutral
}

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void sendTriggerToESP(int sensorId) {
  // Send sensor ID as pulses to ESP32
  for(int i = 0; i < sensorId; i++) {
    digitalWrite(triggerESP, HIGH);
    delay(100);  // 100ms pulse
    digitalWrite(triggerESP, LOW);
    delay(50);   // 50ms gap between pulses
  }
  delay(500); // End signal - indicates transmission complete
}

void loop() {
  long distance1 = readDistance(trigPin1, echoPin1);
  long distance2 = readDistance(trigPin2, echoPin2);

  Serial.print("Slot 1: ");
  Serial.print(distance1);
  Serial.print(" cm | Slot 2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // Update current slot states
  bool current_slot1 = (distance1 < 5);
  bool current_slot2 = (distance2 < 5);

  // Check if slot states changed
  if (current_slot1 != slot1_occupied) {
    slot1_occupied = current_slot1;
    if (!current_slot1) {
      slot1_scanned = false; // Reset scan flag when object leaves
    }
  }

  if (current_slot2 != slot2_occupied) {
    slot2_occupied = current_slot2;
    if (!current_slot2) {
      slot2_scanned = false; // Reset scan flag when object leaves
    }
  }

  // Scanning logic - only scan if object is present and hasn't been scanned
  if (slot1_occupied && slot2_occupied) {
    if (!slot1_scanned || !slot2_scanned) {
      Serial.println("Both slots occupied - scanning both");

      if (!slot1_scanned) {
        Serial.println("Scanning slot 1");
        myServo.write(60);  // Slot 1
        delay(2000);
        sendTriggerToESP(1);  // Send sensor ID 1
        delay(3000);
        slot1_scanned = true;
      }

      if (!slot2_scanned) {
        Serial.println("Scanning slot 2");
        myServo.write(135); // Slot 2
        delay(2000);
        sendTriggerToESP(2);  // Send sensor ID 2
        delay(3000);
        slot2_scanned = true;
      }

      myServo.write(90);  // Return to neutral
    }
  }
  else if (slot1_occupied && !slot1_scanned) {
    Serial.println("Scanning slot 1");
    myServo.write(60);
    delay(2000);
    sendTriggerToESP(1);  // Send sensor ID 1
    delay(3000);
    myServo.write(90);
    slot1_scanned = true;
  }
  else if (slot2_occupied && !slot2_scanned) {
    Serial.println("Scanning slot 2");
    myServo.write(135);
    delay(2000);
    sendTriggerToESP(2);  // Send sensor ID 2
    delay(3000);
    myServo.write(90);
    slot2_scanned = true;
  }
  else if (!slot1_occupied && !slot2_occupied) {
    myServo.write(90);
    Serial.println("No vehicle - Servo at neutral");
  }

  delay(1000);
}