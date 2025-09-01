#include "DimmerControl.h"

#include "Arduino.h"
#include <FunctionalInterrupt.h>

DimmerControl* DimmerControl::s_instance = nullptr;

DimmerControl::DimmerControl(uint8_t triac_pin, uint8_t zc_pin) : triac_pin(triac_pin), zc_pin(zc_pin)
{
}

DimmerControl::~DimmerControl()
{
}

void DimmerControl::begin() {
    DimmerControl::s_instance = this;
    // Pins als Output bzw. Input_Pullup konfigurieren
    pinMode(triac_pin, OUTPUT);
    digitalWrite(triac_pin, LOW); // Sicherstellen, dass der Triac aus ist

    pinMode(zc_pin, INPUT_PULLUP); // ZCD-Pin als Eingang mit Pullup
    // Attach interrupt for zero-cross detection
    // FALLING: Viele ZCD-Module erzeugen einen FALLING-Edge beim Nulldurchgang.
    //          Manchmal auch RISING. Das hängt vom Modul ab.
    timer = timerBegin(0, 80, true);

    timerAttachInterrupt(timer, &DimmerControl::triacPulseISR_wrapper, true);
    attachInterrupt(digitalPinToInterrupt(zc_pin), std::bind(&DimmerControl::zeroCrossISR, this), FALLING);

}

void DimmerControl::setPowerLevel(uint8_t level)
{
    // Edge cases
    if (level <= 0)  {
        currentDelay = delayTimesUs[0];
        return;
    }
    if (level >= 100) {
        currentDelay = delayTimesUs[NUM_LUT_ENTRIES - 1];
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

    currentDelay = (long)interpolatedDelay;
    Serial.printf("Power level: %d: %d %d\n",level, lowerIndex, currentDelay);
}



void DimmerControl::powerOff()
{
    enabled = false;
}

void DimmerControl::powerOn()
{
    enabled = true;
}

void IRAM_ATTR DimmerControl::triacPulseISR_wrapper()
{
    if (DimmerControl::s_instance) {
        DimmerControl::s_instance->triacPulseISR();
    }
}

void IRAM_ATTR DimmerControl::zeroCrossISR()
{
    timerAlarmDisable(timer);
    timerAlarmWrite(timer, currentDelay, false);
    timerAlarmEnable(timer);

}

void DimmerControl::triacPulseISR()
{
    if (!enabled) {
        digitalWrite(triac_pin, LOW);
        return;
    }


    digitalWrite(triac_pin, HIGH);
    delayMicroseconds(MIN_US_PULSE);
    digitalWrite(triac_pin, LOW);

    timerAlarmDisable(timer);
}
