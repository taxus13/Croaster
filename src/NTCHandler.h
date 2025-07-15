#pragma once

#include "NTC_Thermistor.h"
#include "SmoothThermistor.h"

class NTCHandler {
private:
    NTC_Thermistor_ESP32 ntc_reader;
    SmoothThermistor smoothe_reader;
    unsigned long lastSensorRead = 0;
    float temperature = 0;
    const uint8_t pin;
public:
    NTCHandler(uint8_t pin, int readings);
    float getTemperature() const;
    void loop();

};