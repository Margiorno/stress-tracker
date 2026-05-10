#ifndef GSR_PROCESSOR_H
#define GSR_PROCESSOR_H

#include "core/Processor.h"
#include "GsrSensor.h"

class GsrProcessor : public Processor {
private:
    GSRSensor* _sensor;

    unsigned long _lastSampleTime = 0;
    const unsigned long SAMPLE_INTERVAL = 20;
    bool _hasNewData = false;

    float _currentConductance = 0.0;
    float _tonicSCL = 0.0;
    float _phasicSCR = 0.0;
    
    const float ALPHA_TONIC = 0.02; 

    float convertToConductance(long resistance) {
        if (resistance <= 0) return 0.0;
        return 1000000.0 / (float)resistance;
    }

    void initializeFiltersIfNeeded(float conductance) {
        if (_tonicSCL == 0.0 && conductance > 0.0) {
            _tonicSCL = conductance;
        }
    }

    void updateSignalComponents(float conductance) {
        if (conductance <= 0.0) {
            _phasicSCR = 0.0;
            return;
        }

        _tonicSCL = (ALPHA_TONIC * conductance) + ((1.0 - ALPHA_TONIC) * _tonicSCL);
        
        _phasicSCR = conductance - _tonicSCL;
    }

public:
    GsrProcessor(GSRSensor* sensor) : _sensor(sensor) {}

    void process() override {
        unsigned long currentMillis = millis();

        if (currentMillis - _lastSampleTime >= SAMPLE_INTERVAL) {
            _lastSampleTime = currentMillis;

            _sensor->update();

            long rawResistance = _sensor->getResistance();
            _currentConductance = convertToConductance(rawResistance);
            initializeFiltersIfNeeded(_currentConductance);
            updateSignalComponents(_currentConductance);
            
            _hasNewData = true;
        } else {
            _hasNewData = false;
        }
    }

    bool requiresAggregation() override {
        return _hasNewData;
    }

    float getConductance() { return _currentConductance; }
    float getTonic() { return _tonicSCL; }
    float getPhasic() { return _phasicSCR; }
};

#endif