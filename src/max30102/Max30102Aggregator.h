#ifndef MAX30102_AGGREGATOR_H
#define MAX30102_AGGREGATOR_H

#include "core/Aggregator.h"

class Max30102Aggregator : public Aggregator {
private:
    static const int BATCH_SIZE = 50;
    uint32_t _irBatch[BATCH_SIZE];
    uint32_t _redBatch[BATCH_SIZE];
    int _currentIndex;

public:
    Max30102Aggregator() : _currentIndex(0) {}

    void addSample(uint32_t ir, uint32_t red) {
        if (_currentIndex < BATCH_SIZE) {
            _irBatch[_currentIndex] = ir;
            _redBatch[_currentIndex] = red;
            _currentIndex++;
        }
    }

    bool isReady() override {
        return _currentIndex >= BATCH_SIZE;
    }

    void clear() override {
        _currentIndex = 0;
    }

    void appendToPayload(JsonObject& root) override {
        if (_currentIndex == 0) return;

        JsonObject ppgObj = root.createNestedObject("ppg");
        JsonArray irArr = ppgObj.createNestedArray("ir");
        JsonArray redArr = ppgObj.createNestedArray("red");

        for (int i = 0; i < _currentIndex; i++) {
            irArr.add(_irBatch[i]);
            redArr.add(_redBatch[i]);
        }
    }
};

#endif