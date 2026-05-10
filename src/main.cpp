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

// --- MAX30102 Module ---
#include "max30102/Max30102Sensor.h"
#include "max30102/Max30102Processor.h"
#include "max30102/Max30102Aggregator.h"

// --- Payload Builder ---
#include "network/PayloadBuilder.h"

// Network Manager
MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 

// GSR
GSRSensor gsrSensor(34);
GsrProcessor gsrProcessor(&gsrSensor);
GsrAggregator gsrAggregator;

// MAX30102
Max30102Sensor ppgSensor;
Max30102Processor ppgProcessor(&ppgSensor);
Max30102Aggregator ppgAggregator;



Processor* processors[] = { &gsrProcessor, &ppgProcessor };
Aggregator* aggregators[] = { &gsrAggregator, &ppgAggregator };
const int numDevices = 2;

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

void clearI2CBus(int sda, int scl) {
  pinMode(sda, INPUT_PULLUP);
  pinMode(scl, INPUT_PULLUP);
  delay(10);

  if (digitalRead(sda) == LOW) {
    Serial.println("I2C Bus locked! Clearing...");
    pinMode(scl, OUTPUT);
    
    for (int i = 0; i < 9; i++) {
      digitalWrite(scl, LOW);
      delayMicroseconds(20);
      digitalWrite(scl, HIGH);
      delayMicroseconds(20);
      if (digitalRead(sda) == HIGH) {
        //   Serial.println("I2C bus cleared successfully!");
        //   break;
      }
    }
  }

  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
  digitalWrite(sda, LOW);
  delayMicroseconds(20);
  digitalWrite(scl, HIGH);
  delayMicroseconds(20);
  digitalWrite(sda, HIGH);
  delayMicroseconds(20);

  pinMode(sda, INPUT);
  pinMode(scl, INPUT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Checking I2C bus state...");
  clearI2CBus(21, 22);

  Wire.begin();
  Wire.setClock(100000);
//   Wire.setTimeout(20);
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

  Serial.println("Initializing MAX30102 hardware...");
  int retries = 0;
  while (!ppgSensor.begin()) {
      Serial.println("Error: Could not find MAX30102. Retrying in 1s...");
      delay(1000);
      retries++;
      if (retries % 3 == 0) {
          Serial.println("Resetting I2C bus...");
          Wire.end();
          delay(100);
          Wire.begin();
      }
  }

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

  if (ppgProcessor.requiresAggregation()) {
      ppgAggregator.addSample(
          ppgProcessor.getIr(),
          ppgProcessor.getRed()
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
      static uint8_t buffer[4096]; 
      size_t bytesToSend = payloadBuilder.buildBinaryPayload(aggregators, numDevices, buffer, sizeof(buffer));
      
      String topic = String(MQTT_BASE_TOPIC) + "/sensors_batch";
      mqtt.publishBinary(topic.c_str(), buffer, bytesToSend);
      
      for (int i = 0; i < numDevices; i++) {
          aggregators[i]->clear();
      }
  }
}