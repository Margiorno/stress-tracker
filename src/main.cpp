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
#include "Ds18b20Sensor.h"


MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 
GSRSensor gsrSensor(34);
Max30102Sensor sensor;
Mpu6050Sensor mpuSensor;
Ds18b20Sensor ds18b20Sensor(4);

Sensor* sensors[] = {
  &gsrSensor,
  &sensor,
  &mpuSensor,
  &ds18b20Sensor
};
const int numSensors = sizeof(sensors) / sizeof(sensors[0]);

unsigned long lastMsgTime = 0;

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
  clearI2CBus(21, 22);


  Wire.begin();
  Wire.setClock(100000);
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
    int retries = 0;
    while (!sensors[i]->begin()) {        
      Serial.printf("Error: Could not find sensor %s. Retrying in 1s...\n", sensors[i]->getType());
      delay(1000);

      
      retries++;
      if (retries % 3 == 0) {
          Serial.println("Resetting I2C bus...");
          Wire.end();
          delay(100);
          Wire.begin();
      }
    }
    Serial.printf("Sensor %s initialized [OK]\n", sensors[i]->getType());
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


// #include <Arduino.h>
// #include <Wire.h>

// void setup() {
//   Wire.begin();
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("\nStarting I2C scanner...");
// }

// void loop() {
//   byte error, address;
//   int nDevices = 0;

//   Serial.println("Scanning...");

//   for(address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("Found I2C device at address: 0x");
//       if (address < 16) {
//         Serial.print("0");
//       }
//       Serial.print(address, HEX);
//       Serial.println(" !");
//       nDevices++;
//     }
//     else if (error == 4) {
//       Serial.print("Unknown error at address 0x");
//       if (address < 16) {
//         Serial.print("0");
//       }
//       Serial.println(address, HEX);
//     }
//   }
  
//   if (nDevices == 0) {
//     Serial.println("No I2C devices found.\n");
//   } else {
//     Serial.println("Finished scanning.\n");
//   }

//   delay(5000); // Wait 5 seconds before the next scan
// }