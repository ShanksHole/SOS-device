#include <Arduino.h>
#include "WiFi.h" // Initialize Serial1 with custom RX and TX pins
int rxPin = 6; // RX pin
int txPin = 7; // TX pin



// Define pins
#define SOS_BUTTON_PIN 2  // Pin for SOS button

// Function to send command to A9G via serial
void sendCommand(const String& command) {
  Serial1.println(command);  // Send to A9G via Serial1
  delay(100);
}
void makeSOSCall() {
  // Make sure the module is ready to send commands
  sendCommand("AT");  // Test if A9G is responsive
  delay(100);
  
  // Initiate a call to a specified phone number
  sendCommand("ATD+917057558190");  // Replace with target phone number
  delay(5000);  // Wait for the call to establish
  
  // Optional: Hang up the call after a delay
  sendCommand("ATH");  // Hang up the call after a certain time
}

// Function to read response from A9G
String readResponse() {
  String response = "";
  while (Serial1.available()) {
    response += char(Serial1.read());
  }
  return response;
}

// Function to send an SMS with SOS message and current location
void sendSOS() {
  // Get GPS coordinates
  sendCommand("AT+GPS=1");  // Enable GPS
  delay(1000);  // Give some time for GPS to get a fix
  sendCommand("AT+GPSRD=1");  // Start reading GPS data
  delay(2000);  // Wait for GPS data
  
  // Parse GPS data for location
  String gpsData = readResponse();
  String latitude, longitude;
  // Find location data in the GPS response (simplified)
  // The exact parsing depends on the GPS data format
  if (gpsData.indexOf("+GPSRD:") != -1) {
    int start = gpsData.indexOf("+GPSRD:") + 7;
    int end = gpsData.indexOf("\r", start);
    String locationData = gpsData.substring(start, end);
    // Extracting latitude and longitude (example format)
    int latStart = locationData.indexOf(",") + 1;
    int longStart = locationData.indexOf(",", latStart) + 1;
    latitude = locationData.substring(latStart, locationData.indexOf(",", latStart));
    longitude = locationData.substring(longStart, locationData.indexOf(",", longStart));
  }

  // Send SOS message with location via SMS
  sendCommand("AT+CMGF=1");  // Set SMS mode to text
  delay(100);
  sendCommand("AT+CMGS=\"+917057558190\"");  // Replace with target phone number
  delay(100);
  String sosMessage = "SOS! Location: Lat=" + latitude + ", Lon=" + longitude;
  Serial1.print(sosMessage);  // Write the message content
  Serial1.write(26);  // ASCII code for Ctrl+Z to send the message
  delay(1000);
}

void setup() {
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);  // Initialize button pin
  Serial1.begin(115200);  // Initialize serial communication with A9G
  Serial1.begin(115200, SERIAL_8N1, rxPin, txPin);
}

void loop() {
  if (digitalRead(SOS_BUTTON_PIN) == LOW) {  // Button pressed (assuming active low)
    sendSOS();  // Send SOS message
    makeSOSCall();  // Make an SOS call
    delay(1000);  // Avoid multiple sends on a single press (debounce)
  }
}
