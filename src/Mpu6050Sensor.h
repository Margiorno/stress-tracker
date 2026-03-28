#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include "Sensor.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Mpu6050Sensor : public Sensor {
private:
    Adafruit_MPU6050 mpu;
    float _ax, _ay, _az;

public:
    Mpu6050Sensor() : _ax(0), _ay(0), _az(0) {}

    bool begin() override {
        if (!mpu.begin()) {
            Serial.println("MPU6050: Did not find a valid MPU6050 sensor, check wiring!");
            return false;
        }

        mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        
        Serial.println("MPU6050: Initialized successfully.");
        return true;
    }

    void update() override {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        _ax = a.acceleration.x;
        _ay = a.acceleration.y;
        _az = a.acceleration.z;
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