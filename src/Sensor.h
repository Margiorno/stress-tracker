#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
public:
    virtual ~Sensor() {}
    
    virtual bool begin() = 0;
    
    // Fetches new data from the sensor and updates internal state
    virtual void update() = 0;
    
    // Returns data in a format ready for sending via MQTT
    virtual String getData() = 0;
    
    // Name of the sensor used in MQTT message topics
    virtual const char* getType() = 0;
};

#endif