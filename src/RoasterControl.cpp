#include "RoasterControl.h"

#include "HardwareSerial.h"

void RoasterControl::startRoast()
{
    heatLevel = 0;
    fanLevel = 0;
    Serial.printf("startRoast. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::startCool()
{
    heatLevel = 0;
    fanLevel = MAX_LEVEL;
    Serial.printf("startCool. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::stopRoast()
{
    heatLevel = 0;
    fanLevel = 0;
    Serial.printf("stopRoast. Current values H=%d,F=%d\n", heatLevel, fanLevel);

}

void RoasterControl::setHeat(int32_t percentage)
{
    if (percentage > MAX_LEVEL) {
        Serial.printf("Heat percentage too high: %d\n", percentage);
        return;
    }
    if (percentage < MIN_HEAT_LEVEL) {
        Serial.printf("Heat percentage too low: %d\n", percentage);
        return;
    }

    // If heat was enabled with fan disabled, enable fan now
    if (percentage > 0 && fanLevel < MIN_FAN_LEVEL) {
        fanLevel = MIN_FAN_LEVEL;
    }
    heatLevel = percentage;

    Serial.printf("Received heat percentage. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::setFan(int32_t percentage)
{
    if (percentage > MAX_LEVEL) {
        Serial.printf("Fan percentage too high: %d\n", percentage);
    }
    if (percentage < MIN_FAN_LEVEL) {
        Serial.printf("Fan percentage too low: %d\n", percentage);
    }
    fanLevel = percentage;

    Serial.printf("Received fan percentage. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}