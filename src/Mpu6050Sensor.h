#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include "Sensor.h"
#include <Wire.h>

class Mpu6050Sensor : public Sensor {
private:
    float _ax, _ay, _az;
    bool _isInitialized;
    const uint8_t MPU_ADDR = 0x68;

public:
    Mpu6050Sensor() : _ax(0), _ay(0), _az(0), _isInitialized(false) {}

    bool begin() override {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6B); // PWR_MGMT_1 register
        Wire.write(0x80); // Hard reset
        Wire.endTransmission();
        
        delay(100);

        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6B); 
        Wire.write(0x00); // Wake up
        if (Wire.endTransmission() != 0) {
            Serial.println("MPU6050: Init failed!");
            _isInitialized = false;
            return false;
        }

        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x1C); // ACCEL_CONFIG
        Wire.write(0x00); // +/- 2g
        Wire.endTransmission();

        Serial.println("MPU6050: Initialized successfully.");
        _isInitialized = true;
        return true;
    }

    void update() override {
        if (!_isInitialized) return;

        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x3B); // ACCEL_XOUT_H
        
        // Check if the sensor is still responding
        if (Wire.endTransmission(false) != 0) {
            // Sensor didn't ACK, skip this reading to prevent crash
            Serial.println("MPU6050: I2C error during update!");
            return; 
        }
        
        Wire.requestFrom((int)MPU_ADDR, 6, (int)true); 

        if (Wire.available() == 6) {
            int16_t accelX = (Wire.read() << 8 | Wire.read()); 
            int16_t accelY = (Wire.read() << 8 | Wire.read()); 
            int16_t accelZ = (Wire.read() << 8 | Wire.read()); 

            _ax = (accelX / 16384.0) * 9.81;
            _ay = (accelY / 16384.0) * 9.81;
            _az = (accelZ / 16384.0) * 9.81;
        }
    }

    String getData() override {
        String payload = "{";
        payload += "\"x\":" + String(_ax, 2) + ",";
        payload += "\"y\":" + String(_ay, 2) + ",";
        payload += "\"z\":" + String(_az, 2);
        payload += "}";
        return payload;
    }

    const char* getType() override {
        return "MPU6050";
    }

    unsigned long getPublishInterval() override {
        return 200;
    }
};

#endif