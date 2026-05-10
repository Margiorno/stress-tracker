#ifndef MAX30102_PROCESSOR_H
#define MAX30102_PROCESSOR_H

#include "core/Processor.h"
#include "Max30102Sensor.h"

class Max30102Processor : public Processor {
private:
    Max30102Sensor* _sensor;
    unsigned long _lastSampleTime = 0;
    const unsigned long SAMPLE_INTERVAL = 20;
    bool _hasNewData = false;

    uint32_t _currentIr = 0;
    uint32_t _currentRed = 0;

public:
    Max30102Processor(Max30102Sensor* sensor) : _sensor(sensor) {}

    void process() override {
        unsigned long currentMillis = millis();

        if (currentMillis - _lastSampleTime >= SAMPLE_INTERVAL) {
            _lastSampleTime = currentMillis;

            _sensor->update();
            _currentIr = _sensor->getIr();
            _currentRed = _sensor->getRed();
            
            _hasNewData = true;
        } else {
            _hasNewData = false;
        }
    }

    bool requiresAggregation() override {
        return _hasNewData;
    }

    uint32_t getIr() { return _currentIr; }
    uint32_t getRed() { return _currentRed; }
};

#endif