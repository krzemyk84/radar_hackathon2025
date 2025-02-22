#include <Wire.h>
 
#define SLAVE_ADDR 0x08  // Define the slave address (must match the Raspberry Pi's address)
 
void setup() {
  Wire.begin(SLAVE_ADDR);  // Start the I2C slave with the given address
  Wire.onRequest(requestEvent);  // Attach the function to handle requests from master
  Serial.begin(9600);  // Start serial communication for debugging (optional)
}
 
void loop() {
  // Nothing is done here, everything is handled in the requestEvent function
  delay(100);
}
 
// This function is called when the master (Raspberry Pi) requests data
void requestEvent() {
  byte dataToSend = 42;  // Example data to send to the master (you can change this value)
  Wire.write(dataToSend);  // Send the byte to the master
  Serial.print("Data sent: ");
  Serial.println(dataToSend);  // Debugging: print sent data to the Serial Monitor (optional)
}
