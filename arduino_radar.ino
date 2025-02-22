#include <Servo.h>

Servo myservo;  // create Servo object to control a servo
const int pingPin = 7; // Pin for the ultrasonic sensor

int pos = 15;    // variable to store the servo position

void setup() {
  Serial.begin(9600);  // Start serial communication
  myservo.attach(9);   // Attach the servo to pin 9
}

void loop() {
  for (pos = 30; pos <= 150; pos += 15) { // Move the servo from 0 to 180 in 15-degree steps
    myservo.write(pos);              // Move the servo to the position in 'pos'
    delay(500);                      // Wait for the servo to reach the position

    // Get the distance using the ultrasonic sensor
    long duration, inches, cm;

    // Trigger the ultrasonic sensor
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);

    // Read the echo from the sensor
    pinMode(pingPin, INPUT);
    duration = pulseIn(pingPin, HIGH);

    // Convert duration to distance
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);

    // Print the distance to the Serial Monitor
    Serial.print("Angle: ");
    Serial.print(pos);
    Serial.print(" degrees - ");
    Serial.print(inches);
    Serial.print(" inches, ");
    Serial.print(cm);
    Serial.println(" cm");

    delay(500);  // Wait a bit before the next reading
  }

  // Optionally, you could go back to 0 degrees after reaching 180 degrees
  // to continuously scan, or modify as per your needs.
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

