#ifndef GSR_SENSOR_H
#define GSR_SENSOR_H

#include "Sensor.h"

class GSRSensor : public Sensor {
private:
    int _pin;
    int _gsrAverage;
    int _serialCalibration;

    // In documentation, the sensor is described as 10-bit (0-1023), but ESP32's ADC is 12-bit (0-4095).
    // In this case, we need to adjust the reading to fit the expected range. 
    // This method does that by right-shifting the 12-bit value by 2 bits.
    int readAnalog10Bit() {
        return analogRead(_pin) >> 2;
    }

    long calculateResistance(int sensorValue) {
        if (_serialCalibration <= sensorValue) {
            return 0; 
        }
        
        // Equation from documentation:
        // Human Resistance = ((1024 + 2 * Serial_Port_Reading) * 10000) / (Serial_calibration - Serial_Port_Reading)
        return ((1024L + 2L * sensorValue) * 10000L) / (_serialCalibration - sensorValue);
    }

public:
    GSRSensor(int pin) : _pin(pin), _gsrAverage(0), _serialCalibration(512) {}

    bool begin() override {
        pinMode(_pin, INPUT);
        return true;
    }

    // Calibration method to establish a baseline reference value (Serial_calibration) when the sensor is not worn.
    void calibrate() {
        Serial.println("GSR: Calibration starting...");
        long sum = 0;
        
        for(int i = 0; i < 20; i++) {
            sum += readAnalog10Bit();
            delay(50);
        }
        
        _serialCalibration = sum / 20;
        Serial.printf("GSR: Calibration finished (Serial_calibration): %d\n", _serialCalibration);
    }

    void update() override {
        long sum = 0;
        
        // As documentation suggests, we should average multiple readings to get a stable value.
        for(int i = 0; i < 10; i++) {
            sum += readAnalog10Bit();
            delay(5);
        }
        
        _gsrAverage = sum / 10;
    }

    String getData() override {
        long resistance = calculateResistance(_gsrAverage);
        return String(resistance);
    }

    const char* getType() override {
        return "GSR";
    }

    unsigned long getPublishInterval() override {
        return 10;
    }
};

#endif