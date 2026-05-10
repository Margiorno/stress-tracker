// #include <Arduino.h>
// #include <WiFi.h>
// #include <time.h>

// // Environment variables
// #include "env.h"

// // Class for managing MQTT connection and publishing
// #include "MqttManager.h"

// // Class for handling sensors
// #include "GsrSensor.h"
// #include "Max30102Sensor.h"
// #include "Mpu6050Sensor.h"
// #include "Ds18b20Sensor.h"

// #include "DataAggregator.h"



// MqttManager mqtt(MQTT_SERVER, MQTT_PORT, MQTT_BASE_TOPIC); 
// GSRSensor gsrSensor(34);
// Max30102Sensor sensor;
// Mpu6050Sensor mpuSensor;
// Ds18b20Sensor ds18b20Sensor(4);
// DataAggregator aggregator;



// Sensor* sensors[] = {
//   &gsrSensor,
//   &sensor,
//   &mpuSensor,
//   &ds18b20Sensor
// };
// const int numSensors = sizeof(sensors) / sizeof(sensors[0]);
// unsigned long lastSampleTime = 0;
// unsigned long lastTempTime = 0;
// const unsigned long SAMPLE_INTERVAL = 20;
// unsigned long lastMsgTime = 0;

// void clearI2CBus(int sda, int scl) {
//   pinMode(sda, INPUT_PULLUP);
//   pinMode(scl, INPUT_PULLUP);
//   delay(10);

//   if (digitalRead(sda) == LOW) {
//     Serial.println("I2C Bus locked! Clearing...");
//     pinMode(scl, OUTPUT);
    
//     for (int i = 0; i < 9; i++) {
//       digitalWrite(scl, LOW);
//       delayMicroseconds(20);
//       digitalWrite(scl, HIGH);
//       delayMicroseconds(20);
//       if (digitalRead(sda) == HIGH) {
//       }
//     }
//   }

//   pinMode(sda, OUTPUT);
//   pinMode(scl, OUTPUT);
//   digitalWrite(sda, LOW);
//   delayMicroseconds(20);
//   digitalWrite(scl, HIGH);
//   delayMicroseconds(20);
//   digitalWrite(sda, HIGH);
//   delayMicroseconds(20);

//   pinMode(sda, INPUT);
//   pinMode(scl, INPUT);
// }

// void setupTime() {
//   // Configure time servers (0, 0 means no time zone offset - UTC time)
//   configTime(0, 0, "pool.ntp.org", "time.nist.gov");

//   Serial.print("Waiting for NTP time sync");
//   time_t now = time(nullptr);
  
//   // Wait until time is fetched (year must be later than 1970)
//   while (now < 1600000000) {
//     delay(500);
//     Serial.print(".");
//     now = time(nullptr);
//   }
  
//   Serial.println("\nTime synchronized!");
//   struct tm timeinfo;
//   gmtime_r(&now, &timeinfo);
//   Serial.print("Current time (UTC): ");
//   Serial.println(asctime(&timeinfo));
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   clearI2CBus(21, 22);


//   Wire.begin();
//   Wire.setClock(100000);
//   Wire.setTimeout(20);
//   delay(1000);
//   // Serial.println("Starting MQTT test program...");

//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected to WiFi!");

//   setupTime();

//   mqtt.begin(AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE);

//   Serial.println("Initializing sensors...");
//   for (int i = 0; i < numSensors; i++) {
//     int retries = 0;
//     while (!sensors[i]->begin()) {        
//       Serial.printf("Error: Could not find sensor %s. Retrying in 1s...\n", sensors[i]->getType());
//       delay(1000);

      
//       retries++;
//       if (retries % 3 == 0) {
//           Serial.println("Resetting I2C bus...");
//           Wire.end();
//           delay(100);
//           Wire.begin();
//       }
//     }
//     Serial.printf("Sensor %s initialized [OK]\n", sensors[i]->getType());
//   }

//   Serial.println("Calibrating GSR sensor...");
//   gsrSensor.calibrate();



// }

// // void loop() {
// //   mqtt.loop();

// //   for (int i = 0; i < numSensors; i++) {
// //     sensors[i]->update();
// //   }

// //   if (mqtt.isConnected()) {
// //     for (int i = 0; i < numSensors; i++) {
      
// //       if (sensors[i]->shouldPublish()) {
// //         String topic = String(MQTT_BASE_TOPIC) + "/sensor/" + sensors[i]->getType();
// //         Serial.printf("Publishing to topic: %s\n", topic.c_str());
// //         String payload = sensors[i]->getData();

// //         Serial.printf("Sending to %s: %s\n", topic.c_str(), payload.c_str());
// //         mqtt.publish(topic.c_str(), payload.c_str());
// //       }
      
// //     }
// //   }
// // }


// void loop() {
//   mqtt.loop();

//   unsigned long currentMillis = millis();

//   if (currentMillis - lastTempTime >= 1000) {
//     ds18b20Sensor.update();
//     aggregator.setTemperature(ds18b20Sensor.getTemp());
//     lastTempTime = currentMillis;
//   }

//   if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL) {
//     lastSampleTime = currentMillis;
    
//     gsrSensor.update();
//     sensor.update(); 
//     mpuSensor.update();

//     aggregator.addSample(
//       gsrSensor.getResistance(),
//       sensor.getRed(), sensor.getIr(),
//       mpuSensor.getX(), mpuSensor.getY(), mpuSensor.getZ()
//     );

//     if (aggregator.isReady() && mqtt.isConnected()) {
      
//       String payload = aggregator.getJsonString();
//       String topic = String(MQTT_BASE_TOPIC) + "/sensors_batch";
      
//       Serial.println("Sending batch data to MQTT:");
//       mqtt.publish(topic.c_str(), payload.c_str());
      
//       aggregator.clear();
//     }
//   }
// }