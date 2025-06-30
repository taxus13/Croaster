#include "NTCHandler.h"

static constexpr double referenceResistance = 100000;
static constexpr double nominalResistance = 100000;
static constexpr double nominalTemperatureCelsius = 25;
static constexpr double bValue = 3950;
static constexpr uint16_t adcVref = 3300;
static constexpr int adcResolution = 4095;

NTCHandler::NTCHandler(uint8_t pin, int readings) : ntc_reader(pin, referenceResistance, nominalResistance, nominalTemperatureCelsius, adcVref, adcResolution), avg_reader(&ntc_reader, readings) {
    temperature = 0;
}

float NTCHandler::getTemperature() const {
    return temperature;
}



void NTCHandler::loop() {
    unsigned long now = millis();

    if (now - lastSensorRead < 100) {
        return;
    }
    lastSensorRead = now;

    // temperature = avg_reader.readCelsius();
    temperature += 0.05;
}