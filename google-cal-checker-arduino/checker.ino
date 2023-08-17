#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "secrets.h"

// Replace with your network credentials
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

// API details
const char* serverAddress = SECRET_API_SERVER;
const char* apiKey = SECRET_API_KEY;
int port = 443;  // For HTTPS
WiFiSSLClient sslClient;
HttpClient client = HttpClient(sslClient, serverAddress, port);

// Plug the LED into pin 3, change if using a different setup
const int ledPin = 3;
const int ledPin2 = 4;

void setup() {
  pinMode(ledPin, OUTPUT);

  // Begin serial communication
  Serial.begin(115200);

  // Check for the presence of the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not present");
    // don't continue:
    while (true);
  }

  // Attempt to connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    WiFi.begin(ssid, password);

    // Wait 10 seconds for connection
    delay(10000);
  }

  // Connected successfully
  Serial.println("Connected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Making HTTP request...");

    // Make the GET request and get the response
    client.beginRequest();
    client.get("/prod/google-cal-check");
    client.sendHeader("x-api-key", apiKey);
    client.endRequest();

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.println(response);
    Serial.println(statusCode);

    // Manage the lights based on the response
    if (response.indexOf("\"busy\": true") != -1) {

      Serial.println("Busy");
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
      int milliseconds = 5 * 60 * 1000; // check every 5 minutes
      delay(milliseconds);
      Serial.println("Check if still busy...");

    } else if (response.indexOf("\"work_hours\": false") != -1){

      Serial.println("Stop checking until morning");
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
      int milliseconds = 14 * 60 * 60 * 1000; // check back in 14 hours (check at 6pm, then back again at 8am)
      delay(milliseconds);
      Serial.println("Start to check back in the morning...");

    } else if (response.indexOf("\"busy\": false") != -1){

      Serial.println("Not Busy");
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
      int milliseconds = 5 * 60 * 1000; // check every 5 minutes
      delay(milliseconds);
      Serial.println("Check if still not busy...");

    } else {

      Serial.println("Error");
      int blinker = 0;
      while (blinker < 50) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(ledPin2, HIGH);
        delay(1 * 1000); // Blink on and off for 1 second each
        digitalWrite(ledPin, LOW);
        digitalWrite(ledPin2, LOW);
        delay(1 * 1000); // Blink on and off for 1 second each
        blinker = blinker + 1;
      }
    }
  }
}