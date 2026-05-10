#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <ArduinoJson.h>

class Aggregator {
public:
    virtual ~Aggregator() {}

    virtual bool isReady() = 0;
    virtual void clear() = 0;
    
    virtual void appendToPayload(JsonObject& root) = 0;
};

#endif