#ifndef MAX30102_SENSORS_H
#define MAX30102_SENSORS_H

#include "Sensor.h"
#include "DFRobot_MAX30102.h"

class Max30102Sensor : public Sensor {
private:
    DFRobot_MAX30102 _particleSensor;
    uint32_t _redValue;
    uint32_t _irValue;
    bool _isInitialized;

public:
    Max30102Sensor() : _redValue(0), _irValue(0), _isInitialized(false) {}

    bool begin() override {
        if (!_particleSensor.begin()) {
            Serial.println("MAX30102: Init failed!");
            _isInitialized = false;
            return false;
        }

        _particleSensor.sensorConfiguration(
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

        _redValue = _particleSensor.getRed();
        _irValue = _particleSensor.getIR();
    }

    String getData() override {
        String payload = "{";
        payload += "\"red\":" + String(_redValue) + ",";
        payload += "\"ir\":" + String(_irValue);
        payload += "}";
        return payload;
    }

    const char* getType() override {
        return "MAX30102";
    }

    unsigned long getPublishInterval() override {
        return 20;
    }
};




#endif