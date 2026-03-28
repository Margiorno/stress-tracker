#include <Arduino.h>
#include <WiFi.h>

// Environment variables
#include "env.h"

// Class for managing MQTT connection and publishing
#include "MqttManager.h"

// Class for handling sensors
#include "GsrSensor.h"
#include "Max30102Sensor.h"
#include "Mpu6050Sensor.h"


MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 
GSRSensor gsrSensor(34);
Max30102RedSensor redSensor;
Max30102IrSensor irSensor;
Mpu6050Sensor mpuSensor;

Sensor* sensors[] = {
  &gsrSensor,
  &redSensor,
  &irSensor,
  &mpuSensor
};
const int numSensors = sizeof(sensors) / sizeof(sensors[0]);

unsigned long lastMsgTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(1000);
  // Serial.println("Starting MQTT test program...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  mqtt.begin();

  Serial.println("Initializing sensors...");
  for (int i = 0; i < numSensors; i++) {
    sensors[i]->begin();
  }

  Serial.println("Calibrating GSR sensor...");
  gsrSensor.calibrate();



}

void loop() {
  mqtt.loop();

  for (int i = 0; i < numSensors; i++) {
    sensors[i]->update();
  }

  if (mqtt.isConnected()) {
    for (int i = 0; i < numSensors; i++) {
      
      if (sensors[i]->shouldPublish()) {
        String topic = String(MQTT_BASE_TOPIC) + "/sensor/" + sensors[i]->getType();
        // Serial.printf("Publishing to topic: %s\n", topic.c_str());
        String payload = sensors[i]->getData();

        // Serial.printf("Sending to %s: %s\n", topic.c_str(), payload.c_str());
        mqtt.publish(topic.c_str(), payload.c_str());
      }
      
    }
  }
}