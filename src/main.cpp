#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

// --- Configuration & Network ---
#include "config/env.h"
#include "network/MqttManager.h"

// --- Interfaces ---
#include "core/Processor.h"
#include "core/Aggregator.h"

// --- GSR Module ---
#include "gsr/GsrSensor.h"
#include "gsr/GsrProcessor.h"
#include "gsr/GsrAggregator.h"

// --- Payload Builder ---
#include "network/PayloadBuilder.h"

// Network Manager
MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 

// GSR
GSRSensor gsrSensor(34);
GsrProcessor gsrProcessor(&gsrSensor);
GsrAggregator gsrAggregator;



Processor* processors[] = { &gsrProcessor };
Aggregator* aggregators[] = { &gsrAggregator };
const int numDevices = 1;

PayloadBuilder payloadBuilder;

void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync");
  time_t now = time(nullptr);
  while (now < 1600000000) { 
      delay(500); 
      Serial.print("."); 
      now = time(nullptr); 
  }
  Serial.println("\nTime synchronized!");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { 
      delay(500); 
      Serial.print("."); 
  }
  Serial.println("\nConnected to WiFi!");

  setupTime();

  mqtt.begin(AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE);

  Serial.println("Initializing GSR hardware...");
  gsrSensor.begin();

  Serial.println("Wait 3 seconds before calibration...");
  delay(3000);
  gsrSensor.calibrate();
}

void loop() {
  mqtt.loop();

  for (int i = 0; i < numDevices; i++) {
      processors[i]->process();
  }

  if (gsrProcessor.requiresAggregation()) {
      gsrAggregator.addSample(
          gsrProcessor.getConductance(),
          gsrProcessor.getTonic(),
          gsrProcessor.getPhasic()
      );
  }

  bool allReady = true;
  for (int i = 0; i < numDevices; i++) {
      if (!aggregators[i]->isReady()) {
          allReady = false;
          break;
      }
  }

  if (allReady && mqtt.isConnected()) {
      uint8_t buffer[1024]; 
      size_t bytesToSend = payloadBuilder.buildBinaryPayload(aggregators, numDevices, buffer, sizeof(buffer));
      
      String topic = String(MQTT_BASE_TOPIC) + "/sensors_batch";
      mqtt.publishBinary(topic.c_str(), buffer, bytesToSend);
      
      for (int i = 0; i < numDevices; i++) {
          aggregators[i]->clear();
      }
  }
}