#ifndef GSR_SENSOR_H
#define GSR_SENSOR_H

#include "core/Sensor.h"

class GSRSensor : public Sensor {
private:
    int _pin;
    long _serialCalibration;
    long _currentResistance = 0;

    int readAnalog10Bit() {
        return analogRead(_pin) >> 2;
    }

public:
    GSRSensor(int pin) : _pin(pin), _serialCalibration(512) {}

    bool begin() override {
        pinMode(_pin, INPUT);
        return true;
    }

    void calibrate() {
        Serial.println("GSR: Starting calibration...");
        long sum = 0;
        for(int i = 0; i < 20; i++) {
            sum += readAnalog10Bit();
            delay(50);
        }
        _serialCalibration = sum / 20;
        Serial.printf("GSR: Calibration finished. Base calibration: %ld\n", _serialCalibration);
    }

    void update() override {
        long sum = 0;
        for(int i = 0; i < 10; i++) {
            sum += readAnalog10Bit();
        }
        int avgRaw = sum / 10;

        // Serial.print("Raw ADC: ");
        // Serial.println(avgRaw);

        if (_serialCalibration > avgRaw) {
            _currentResistance = ((1024L + 2L * avgRaw) * 10000L) / (_serialCalibration - avgRaw);
        } else {
            _currentResistance = 0;
        }
    }

    long getResistance() { return _currentResistance; }

    String getData() override { return ""; }
    const char* getType() override { return "GSR"; }
    unsigned long getPublishInterval() override { return 1000; }
};

#endif