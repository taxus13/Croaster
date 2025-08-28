#include "DimmerControl.h"

#include "Arduino.h"
#include <FunctionalInterrupt.h>

DimmerControl::DimmerControl(uint8_t triac_pin, uint8_t zc_pin) : triac_pin(triac_pin), zc_pin(zc_pin)
{
}

DimmerControl::~DimmerControl()
{
}

void DimmerControl::begin() {
    // Pins als Output bzw. Input_Pullup konfigurieren
    pinMode(triac_pin, OUTPUT);
    digitalWrite(triac_pin, LOW); // Sicherstellen, dass der Triac aus ist

    pinMode(zc_pin, INPUT_PULLUP); // ZCD-Pin als Eingang mit Pullup
    // Attach interrupt for zero-cross detection
    // FALLING: Viele ZCD-Module erzeugen einen FALLING-Edge beim Nulldurchgang.
    //          Manchmal auch RISING. Das hängt vom Modul ab.
    attachInterrupt(digitalPinToInterrupt(zc_pin), std::bind(&DimmerControl::zeroCrossISR, this), FALLING);

    xTaskCreatePinnedToCore(
            &DimmerControl::startDimmerTask,
            "FanDimmerTask",
            4096,
            this,
            1,
            &startDimmerTask,
            1
        );

}

void DimmerControl::setPowerLevel(uint8_t level)
{
    // Edge cases
    if (level <= 0)  {
        currentDealy = delayTimesUs[0];
        return;
    }
    if (level >= 100) {
        currentDealy = delayTimesUs[NUM_LUT_ENTRIES - 1];
        return;
    }

    // Finde die passenden Indizes in der LUT
    int lowerIndex = 0;
    for (int i = 0; i < NUM_LUT_ENTRIES - 1; i++) {
        if (level >= powerLevels[i] && level < powerLevels[i+1]) {
            lowerIndex = i;
            break;
        }
    }

    // Lineare Interpolation zwischen den gefundenen Punkten
    int upperIndex = lowerIndex + 1;
    float interpolatedDelay = map(
        level,
        powerLevels[lowerIndex],
        powerLevels[upperIndex],
        delayTimesUs[lowerIndex],
        delayTimesUs[upperIndex]
    );

    currentDealy = (long)interpolatedDelay;
}

void DimmerControl::dimmerControl()
{
    while (true) {
        if (!enabled) {
            digitalWrite(triac_pin, LOW);
            delayMicroseconds(100);
            continue;
        }
        if (enabled && zc_detected) {    
            zc_detected = false;
            int delayTimeUs = currentDealy;
            if (delayTimeUs < HALF_CYCLE_US - MIN_US_PULSE) {
                delayMicroseconds(delayTimeUs);
                digitalWrite(triac_pin, HIGH);
                delayMicroseconds(MIN_US_PULSE);
                digitalWrite(triac_pin, LOW);
            } else {
                digitalWrite(triac_pin, LOW);
            }
        } else {
            delayMicroseconds(10);

        }
        
    }
}

void DimmerControl::powerOff()
{
    enabled = false;
}

void DimmerControl::powerOn()
{
    enabled = true;
}

void DimmerControl::startDimmerTask(void *param)
{
    Serial.println("Dimmer task not running, starting...");

    DimmerControl* self = static_cast<DimmerControl*>(param);
    self->dimmerControl();
}

void IRAM_ATTR DimmerControl::zeroCrossISR() {
    zc_detected = true;
}