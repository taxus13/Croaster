#include "NTCHandler.h"

static constexpr double referenceResistance = 100000;
static constexpr double nominalResistance = 90000;
static constexpr double nominalTemperatureCelsius = 25;
static constexpr double bValue = 3950;
static constexpr uint16_t adcVref = 3300;
static constexpr int adcResolution = 4095;
static constexpr float V_REF = 3.3;

NTCHandler::NTCHandler(uint8_t pin, int readings) : ntc_reader(pin, referenceResistance, nominalResistance, nominalTemperatureCelsius, adcVref, adcResolution), smoothe_reader(&ntc_reader, 10), pin(pin) {
    temperature = 0;
    analogReadResolution(12);
}

float NTCHandler::getTemperature() const {
    return temperature;
}



void NTCHandler::loop() {
    unsigned long now = millis();

    if (now - lastSensorRead < 1000) {
        return;
    }
    lastSensorRead = now;


    temperature = smoothe_reader.readCelsius();
    // Serial.printf("Read Temp: %fC, Voltage: %f, Value: %d\n", temperature, 0.0, 0.0);

}