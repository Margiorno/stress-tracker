#ifndef MAX30102_RAW_SENSOR_H
#define MAX30102_RAW_SENSOR_H

#include "Sensor.h"
#include "DFRobot_MAX30102.h"

class Max30102RawSensor : public Sensor {
private:
    DFRobot_MAX30102 particleSensor;
    uint32_t _redValue;
    uint32_t _irValue;

public:
    Max30102RawSensor() : _redValue(0), _irValue(0) {}

    bool begin() override {
        if (!particleSensor.begin()) {
            Serial.println("MAX30102: Nie znaleziono czujnika!");
            return false;
        }

        particleSensor.sensorConfiguration(
            60,               // ledBrightness
            SAMPLEAVG_8,      // sampleAverage
            MODE_MULTILED,    // ledMode (Red i IR same time)
            SAMPLERATE_400,   // sampleRate
            PULSEWIDTH_411,   // pulseWidth
            ADCRANGE_16384    // adcRange (wide range for better resolution)
        );
        
        Serial.println("MAX30102: Initialized in RAW mode.");
        return true;
    }

    void update() override {
        _irValue = particleSensor.getIR();
        _redValue = particleSensor.getRed();
    }

    String getData() override {
        String payload = "{";
        payload += "\"red\":" + String(_redValue) + ",";
        payload += "\"ir\":" + String(_irValue);
        payload += "}";
        return payload;
    }

    const char* getType() override {
        return "MAX30102_RAW";
    }
};

#endif