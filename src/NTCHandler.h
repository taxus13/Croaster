#pragma once

#include "NTC_Thermistor.h"
#include "AverageThermistor.h"

class NTCHandler {
private:
    NTC_Thermistor_ESP32 ntc_reader;
    AverageThermistor avg_reader;
    unsigned long lastSensorRead = 0;
    float temperature = 0;
public:
    NTCHandler(uint8_t pin, int readings);
    float getTemperature() const;
    void loop();

};