#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
protected:
    unsigned long _lastSendTime = 0;
    
public:
    virtual ~Sensor() {}
    
    virtual bool begin() = 0;
    
    // Fetches new data from the sensor and updates internal state
    virtual void update() = 0;
    
    // Returns data in a format ready for sending via MQTT
    virtual String getData() = 0;
    
    // Name of the sensor used in MQTT message topics
    virtual const char* getType() = 0;

    // Optional: Define how often this sensor should publish data (in milliseconds)
    virtual unsigned long getPublishInterval() = 0;

    bool shouldPublish() {
        if (millis() - _lastSendTime >= getPublishInterval()) {
            _lastSendTime = millis();
            return true;
        }
        return false;
    }
};

#endif