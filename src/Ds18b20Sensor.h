#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include "Sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20Sensor : public Sensor {
private:
    int _pin;
    float _temperature;
    bool _isInitialized;

    OneWire _oneWire;
    DallasTemperature _sensor;

public:
    Ds18b20Sensor(int pin) : 
        _pin(pin), 
        _temperature(0.0), 
        _isInitialized(false), 
        _oneWire(pin), 
        _sensor(&_oneWire) {}

    bool begin() override {
        _sensor.begin();

        if (_sensor.getDeviceCount() == 0) {
            Serial.print("DS18B20: Did not find any DS18B20 sensor on pin ");
            Serial.println(_pin);
            _isInitialized = false;
            return false;
        }

        Serial.println("DS18B20: Initialized successfully.");
        _isInitialized = true;
        return true;
    }

    void update() override {
        if (!_isInitialized) return;

        _sensor.requestTemperatures();

        float tempC = _sensor.getTempCByIndex(0);

        if (tempC != DEVICE_DISCONNECTED_C) {
            _temperature = tempC;
        } else {
            Serial.println("DS18B20: Error reading temperature!");
        }
    }

    String getData() override {
        return String(_temperature, 3);
    }

    const char* getType() override {
        return "DS18B20";
    }

    unsigned long getPublishInterval() override {
        return 100;
    }
};

#endif