#include <FastLED.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "config.h"  // Include configuration file

#define LED_PIN     6
#define NUM_LEDS    256
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// API Configuration
const char* apiEndpoint = "/api/arduinotest";

void setup() {
  delay(2000);
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial
  }
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.println("\nAPI Test Starting...");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!requestMade && WiFi.status() == WL_CONNECTED) {
    makeApiRequest();
    requestMade = true;
    Serial.println("\nTest complete. Reset board to test again.");
  }
  delay(1000);
}

void makeApiRequest() {
  WiFiClient client;
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  Serial.println("Making API Request...");
  Serial.print("Connecting to: ");
  Serial.println(apiHost);
  
  if (client.connect(apiHost, 80)) {
    Serial.println("Connected to server\n");
    
    // Make HTTP request
    client.print("GET ");
    client.print(apiEndpoint);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(apiHost);
    client.println("Accept: application/json");
    client.println("Connection: close");
    client.println();
    
    Serial.println("Waiting for response...\n");
    
    // First, print raw response for debugging
    Serial.println("--- Raw Response Start ---");
    String rawResponse = "";
    bool headersEnded = false;
    
    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        
        // Check for end of headers
        if (line == "\r") {
          headersEnded = true;
          Serial.println("--- Headers End, JSON Start ---");
          continue;
        }
        
        // Store only the JSON part
        if (headersEnded) {
          rawResponse += line + "\n";
        }
      }
    }
    
    Serial.println("--- Raw Response End ---");
    Serial.println("\nAttempting to parse JSON:");
    Serial.println(rawResponse);
    
    // Now try to parse the JSON
    DeserializationError error = deserializeJson(doc, rawResponse);
    
    if (!error) {
      Serial.println("\nJSON parsed successfully!");
      
      // Access and print some values to verify parsing
      Serial.println("\nPosts:");
      JsonArray posts = doc["posts"].as<JsonArray>();
      for(JsonVariant v : posts) {
        Serial.print("  ID: ");
        Serial.print(v["id"].as<int>());
        Serial.print(", Title: ");
        Serial.println(v["title"].as<const char*>());
      }
      
      Serial.println("\nProfile:");
      Serial.print("  Name: ");
      Serial.println(doc["profile"]["name"].as<const char*>());
      
    } else {
      Serial.print("\nJSON parsing failed: ");
      Serial.println(error.c_str());
    }
    
    client.stop();
  } else {
    Serial.println("Connection to server failed!");
  }
} 