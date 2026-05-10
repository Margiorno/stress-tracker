#ifndef MAX30102_SENSOR_H
#define MAX30102_SENSOR_H

#include "core/Sensor.h"
#include <DFRobot_MAX30102.h>

class Max30102Sensor : public Sensor {
private:
    DFRobot_MAX30102 _sensor;
    uint32_t _currentIr = 0;
    uint32_t _currentRed = 0;
    bool _isInitialized = false;

public:
    bool begin() override {
        if (!_sensor.begin()) {
            Serial.println("MAX30102: Init failed!");
            _isInitialized = false;
            return false;
        }
        
        _sensor.sensorConfiguration(
            60,              // ledBrightness
            SAMPLEAVG_8,     // sampleAverage 
            MODE_MULTILED,   // ledMode
            SAMPLERATE_400,  // sampleRate
            PULSEWIDTH_411,  // pulseWidth
            ADCRANGE_16384   // adcRange
        );

        Serial.println("MAX30102: Initialized successfully.");

        _isInitialized = true;

        return true;
    }

    void update() override {
        if (!_isInitialized) return;
        
        _sensor.getBoth(&_currentIr, &_currentRed);
    }

    uint32_t getIr() { return _currentIr; }
    uint32_t getRed() { return _currentRed; }

    String getData() override { return ""; }
    const char* getType() override { return "MAX30102"; }
    unsigned long getPublishInterval() override { return 1000; }
};

#endif