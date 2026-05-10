#ifndef GSR_AGGREGATOR_H
#define GSR_AGGREGATOR_H

#include "core/Aggregator.h"

class GsrAggregator : public Aggregator {
private:
    static const int BATCH_SIZE = 50;

    float _conductanceBatch[BATCH_SIZE];
    float _tonicBatch[BATCH_SIZE];
    float _phasicBatch[BATCH_SIZE];
    int _currentIndex;

public:
    GsrAggregator() : _currentIndex(0) {}

    void addSample(float conductance, float tonic, float phasic) {
        if (_currentIndex < BATCH_SIZE) {
            _conductanceBatch[_currentIndex] = conductance;
            _tonicBatch[_currentIndex] = tonic;
            _phasicBatch[_currentIndex] = phasic;
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

        JsonObject gsrObj = root.createNestedObject("gsr");
        JsonArray condArr = gsrObj.createNestedArray("raw_uS");
        JsonArray tonicArr = gsrObj.createNestedArray("scl_tonic");
        JsonArray phasicArr = gsrObj.createNestedArray("scr_phasic");

        for (int i = 0; i < _currentIndex; i++) {
            condArr.add(_conductanceBatch[i]);
            tonicArr.add(_tonicBatch[i]);
            phasicArr.add(_phasicBatch[i]);
        }
    }
};

#endif