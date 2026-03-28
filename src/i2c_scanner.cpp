// To start scanning, uncomment the following code and upload to your ESP32. Open the Serial Monitor to see the results.

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