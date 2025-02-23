#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDR 0x08  // Slave I2C address
const int pingPin = 7;  // Pin for the ultrasonic sensor
const int buzzerPin = 8;  // Pin for the buzzer
Servo myservo;         // Create Servo object to control a servo

int pos = 15;           // Variable to store the servo position
long distanceCm;        // Distance in centimeters

// Define maximum distance
const int max_distance = 200; // Maximum distance in cm

void setup() {
  Serial.begin(9600);    // Start serial communication for debugging (optional)
  Wire.begin(SLAVE_ADDR);  // Start the I2C slave with the defined address
  Wire.onRequest(requestEvent);  // Attach the request event function
  myservo.attach(9);         // Attach the servo to pin 9
  pinMode(buzzerPin, OUTPUT);  // Set buzzerPin as an output
}

void loop() {
  for (pos = 30; pos <= 150; pos += 15) {  // Sweep the servo from 30 to 150 degrees
    myservo.write(pos);  // Move the servo to the current position
    delay(500);  // Wait for the servo to reach the position

    // Get the distance from the ultrasonic sensor
    long duration = getDistance();

    // Convert duration to distance in centimeters
    distanceCm = microsecondsToCentimeters(duration);

    // Debugging: Print the distance and servo position
    Serial.print("Angle: ");
    Serial.print(pos);
    Serial.print(" degrees - ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    // Check if the detected distance is within a valid range and trigger a beep
    if (distanceCm > 0 && distanceCm <= max_distance) {  // Check for valid distance
      int frequency = getFrequencyForDistance(distanceCm);  // Get frequency based on distance
      tone(buzzerPin, frequency, 100);  // Play sound for 100ms (short beep)
    }

    delay(500);  // Wait before moving to the next position
  }
}

void requestEvent() {
  // Send the ultrasonic data (centimeters) and servo position to the master
  //Wire.write((byte*)&distanceCm, sizeof(distanceCm));          // Send distance in centimeters
  //Wire.write((byte*)&pos, sizeof(pos));                        // Send the current servo position
  Wire.write(pos);
  Wire.write(distanceCm);

  // Debugging: Print sent data to the Serial Monitor
  Serial.print("Data sent: ");
  Serial.print("Cm: ");
  Serial.print(distanceCm);
  Serial.print(", Servo: ");
  Serial.println(pos);
}

// Function to get the correct frequency based on the distance
int getFrequencyForDistance(long distanceCm) {
  // Map the distance to a frequency range (higher frequency for closer objects)
  int frequency = map(distanceCm, 0, max_distance, 2000, 500);  // Higher frequency for closer objects
  return frequency;
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

// Function to convert the pulse duration to distance in centimeters
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;  // Conversion factor for distance in centimeters
}
