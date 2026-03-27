#ifndef MAX30102_SENSORS_H
#define MAX30102_SENSORS_H

#include "Sensor.h"
#include "DFRobot_MAX30102.h"

// Współdzielony obiekt fizycznego czujnika
static DFRobot_MAX30102 particleSensor;
static bool isMax30102Initialized = false;

// ------------------------------------------
// KLASA 1: Czujnik fali czerwonej (RED)
// ------------------------------------------
class Max30102RedSensor : public Sensor {
private:
    uint32_t _redValue;

public:
    Max30102RedSensor() : _redValue(0) {}

    bool begin() override {
        if (!isMax30102Initialized) {
            if (!particleSensor.begin()) return false;
            // Konfiguracja pod szybkie wykresy (odczyt surowy)
            particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED, SAMPLERATE_400, PULSEWIDTH_411, ADCRANGE_16384);
            isMax30102Initialized = true;
        }
        return true;
    }

    void update() override { _redValue = particleSensor.getRed(); }
    
    String getData() override { return String(_redValue); } // Czysty, prosty payload!
    
    const char* getType() override { return "MAX30102_RED"; }
    
    unsigned long getPublishInterval() override { return 100; } // Wysyłka co 100 ms (10 razy na sek.)
};

// ------------------------------------------
// KLASA 2: Czujnik fali podczerwonej (IR)
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

    void update() override { _irValue = particleSensor.getIR(); }
    
    String getData() override { return String(_irValue); } // Czysty, prosty payload!
    
    const char* getType() override { return "MAX30102_IR"; }
    
    unsigned long getPublishInterval() override { return 100; } // Wysyłka co 100 ms
};

#endif