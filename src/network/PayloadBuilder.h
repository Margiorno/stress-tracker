#ifndef PAYLOAD_BUILDER_H
#define PAYLOAD_BUILDER_H

#include <ArduinoJson.h>
#include "core/Aggregator.h"

class PayloadBuilder {
public:
    size_t buildBinaryPayload(Aggregator** aggregators, int numAggregators, uint8_t* buffer, size_t bufferSize) {
        
        DynamicJsonDocument doc(4096);
        JsonObject root = doc.to<JsonObject>();

        for (int i = 0; i < numAggregators; i++) {
            aggregators[i]->appendToPayload(root);
        }

        return serializeMsgPack(doc, buffer, bufferSize);
    }
};

#endif