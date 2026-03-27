#include <Arduino.h>
#include <WiFi.h>

// Environment variables
#include "env.h"

// Class for managing MQTT connection and publishing
#include "MqttManager.h"

MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 

unsigned long lastMsgTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting MQTT test program...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  mqtt.begin();
}

void loop() {
  mqtt.loop();

  if (millis() - lastMsgTime > 5000) {
    lastMsgTime = millis();

    if (mqtt.isConnected()) {
      Serial.println("Sending test message: cosik");

      mqtt.publish("mrar/test", "cosik");
    }
  }
}