#define BLYNK_PRINT Serial  // Enable serial output for Blynk debugging

// Define Blynk template ID, template name, and authentication token
#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "IOT Water Management System"
#define BLYNK_AUTH_TOKEN "************************"

// Include necessary libraries for Wi-Fi and Blynk functionality
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi credentials
char ssid[] = "ssid";        // Wi-Fi SSID
char pass[] = "password";    // Wi-Fi Password

// Define pin assignments
int trigger = D2;            // Trigger pin for ultrasonic sensor
int echo = D3;               // Echo pin for ultrasonic sensor
int connected = D5;          // Pin to indicate Wi-Fi connection
int disconnected = D6;       // Pin to indicate Wi-Fi disconnection
int pump = D8;               // Pin to control the water pump
int waterLevel;              // Variable to store water level
float reservoirHeight = 14;  // Height of the reservoir in cm

// Create a widget for the pump state on the Blynk app
WidgetLED pumpState(V0);

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate

  // Set pin modes for output and input
  pinMode(connected, OUTPUT);
  pinMode(disconnected, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  // Initialize Blynk with the authentication token and Wi-Fi credentials
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Call function to indicate connection status
  connectedFunc();

  // Print message to the serial monitor
  Serial.println("");
  Serial.println("WiFi Connected");
}

// Function to indicate that the device is connected to Wi-Fi
void connectedFunc() {
  digitalWrite(disconnected, LOW);  // Turn off disconnected indicator
  digitalWrite(connected, HIGH);     // Turn on connected indicator
}

// Function to indicate that the device is disconnected from Wi-Fi
void disconnectedFunc() {
  digitalWrite(disconnected, HIGH);  // Turn on disconnected indicator
  digitalWrite(connected, LOW);      // Turn off connected indicator
}

// Function to get the water level using an ultrasonic sensor
float getWaterLevel() {
  digitalWrite(trigger, LOW);              // Set trigger to low
  delayMicroseconds(2);                    // Wait for 2 microseconds

  digitalWrite(trigger, HIGH);             // Set trigger to high
  delayMicroseconds(10);                   // Wait for 10 microseconds

  digitalWrite(trigger, LOW);              // Set trigger back to low

  // Calculate and return the distance in cm
  return (pulseIn(echo, HIGH) * 0.034 / 2.0);
}

// Function to turn on the pump and print a message
void onPump() {
  digitalWrite(pump, HIGH);                // Turn on the pump
  Serial.println("PUMP HAS BEEN TURNED ON"); // Print message to serial monitor
}

// Function to turn off the pump and print a message
void offPump() {
  digitalWrite(pump, LOW);                 // Turn off the pump
  Serial.println("PUMP HAS BEEN TURNED OFF"); // Print message to serial monitor
}

// Function to map a value from one range to another
int mapValue(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; // Return mapped value
}

// Blynk button handler for controlling the pump from the app
BLYNK_WRITE(V0) {
  int buttonState = param.asInt();  // Get the state of the button (on/off)

  // Turn on or off the pump based on button state
  if (buttonState == 1) {
    onPump();
  } else {
    offPump();
  }
}

// Main loop function
void loop() {
  Blynk.run();  // Run Blynk functions

  // Get water level and map it to a percentage (0-100)
  waterLevel = mapValue(getWaterLevel(), reservoirHeight, 3, 0, 100);

  // Control pump based on water level
  if (waterLevel <= 20) {
    onPump();            // Turn on pump if water level is low
    pumpState.on();     // Update pump state on Blynk app
  } else if (waterLevel > 90) {
    offPump();          // Turn off pump if water level is high
    pumpState.off();    // Update pump state on Blynk app
  }

  // Send the water level to Blynk app for virtual pins V1 and V2
  Blynk.virtualWrite(V1, waterLevel);
  Blynk.virtualWrite(V2, waterLevel);

  // Print the water level to the serial monitor
  Serial.println(waterLevel);

  // Check Wi-Fi connection status
  if (WiFi.status() == WL_CONNECTED) {
    connectedFunc();  // Call function to indicate connection
  } else {
    disconnectedFunc(); // Call function to indicate disconnection
  }
  delay(1500);  // Wait for 1.5 seconds before next loop iteration
}
