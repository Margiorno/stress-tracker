#ifndef MAX30102_SENSORS_H
#define MAX30102_SENSORS_H

#include "Sensor.h"
#include "DFRobot_MAX30102.h"

// Shared object for the physical sensor
static DFRobot_MAX30102 particleSensor;
static bool isMax30102Initialized = false;

// ------------------------------------------
// CLASS 1: Sensor for Red Light (RED)
// ------------------------------------------
class Max30102RedSensor : public Sensor {
private:
    uint32_t _redValue;

public:
    Max30102RedSensor() : _redValue(0) {}

    bool begin() override {
        if (!isMax30102Initialized) {
            if (!particleSensor.begin()) return false;
            // Configuration for fast graphs (raw reading)
            particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED, SAMPLERATE_400, PULSEWIDTH_411, ADCRANGE_16384);
            isMax30102Initialized = true;
        }
        return true;
    }

    void update() override {
        _redValue = particleSensor.getRed();
    }
    
    String getData() override {
        return String(_redValue);
    }
    
    const char* getType() override {
        return "MAX30102_RED";
    }
    
    unsigned long getPublishInterval() override {
        return 20;
    }
};

// ------------------------------------------
// CLASS 2: Infrared Light Sensor (IR)
// ------------------------------------------
class Max30102IrSensor : public Sensor {
private:
    uint32_t _irValue;

public:
    Max30102IrSensor() : _irValue(0) {}

    bool begin() override {
        if (!isMax30102Initialized) {
            if (!particleSensor.begin()) return false;
            particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED, SAMPLERATE_400, PULSEWIDTH_411, ADCRANGE_16384);
            isMax30102Initialized = true;
        }
        return true;
    }

    void update() override {
        _irValue = particleSensor.getIR();
    }

    String getData() override {
        return String(_irValue);
    }

    const char* getType() override {
        return "MAX30102_IR";
    }

    unsigned long getPublishInterval() override {
        return 20;
    }
};

#endif