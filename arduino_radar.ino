#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDR 0x08  // Slave I2C address
const int pingPin = 7; // Pin for the ultrasonic sensor
Servo myservo;        // Create Servo object to control a servo

int pos = 15;          // Variable to store the servo position
long distanceInches, distanceCm;

void setup() {
  Serial.begin(9600);   // Start serial communication for debugging (optional)
  Wire.begin(SLAVE_ADDR);  // Start the I2C slave with the defined address
  Wire.onRequest(requestEvent);  // Attach the request event function
  myservo.attach(9);       // Attach the servo to pin 9
}

void loop() {
  for (pos = 30; pos <= 150; pos += 15) {  // Sweep the servo from 30 to 150 degrees
    myservo.write(pos);  // Move the servo to the current position
    delay(500);  // Wait for the servo to reach the position

    // Get the distance from the ultrasonic sensor
    long duration = getDistance();

    // Convert duration to distance in inches and centimeters
    distanceInches = microsecondsToInches(duration);
    distanceCm = microsecondsToCentimeters(duration);

    // Debugging: Print the distance and servo position
    Serial.print("Angle: ");
    Serial.print(pos);
    Serial.print(" degrees - ");
    Serial.print(distanceInches);
    Serial.print(" inches, ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    delay(500);  // Wait a bit before the next servo movement
  }
}

void requestEvent() {
  // Send the ultrasonic data (inches and centimeters) and servo position to the master
  //Wire.write(distanceInches);   // Send the distance in inches (1 byte)
  
  Wire.write(pos);               // Send the current servo position (1 byte)
  Wire.write(distanceCm);        // Send the distance in centimeters (1 byte)

  // Debugging: Print sent data to the Serial Monitor
  Serial.print("Data sent: ");
  //Serial.print("Inches: ");
  //Serial.print(distanceInches);
  Serial.print(", Cm: ");
  Serial.print(distanceCm);
  Serial.print(", Servo: ");
  Serial.println(pos);
}

// Function to trigger the ultrasonic sensor and get the distance
long getDistance() {
  // Trigger the ultrasonic sensor
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // Read the echo from the sensor
  pinMode(pingPin, INPUT);
  long duration = pulseIn(pingPin, HIGH);  // Get the pulse duration

  return duration;
}

// Functions to convert the pulse duration to distance in inches or centimeters
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
