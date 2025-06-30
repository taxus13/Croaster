#include "RoasterControl.h"

#include "HardwareSerial.h"
#include "PinConfig.h"

RoasterControl::RoasterControl(int32_t cycleFan_ms, int32_t cycleHeat_ms) : cycleFan_ms(cycleFan_ms), cycleHeat_ms(cycleHeat_ms)
{
}

void RoasterControl::begin()
{
    pinMode(SSR_FAN_PIN, OUTPUT);
    pinMode(SSR_HEATER_PIN, OUTPUT);
    // xTaskCreatePinnedToCore(
    //     &RoasterControl::startFanSSR,
    //     "FanSSRTask",
    //     4096,
    //     this,
    //     1,
    //     NULL,
    //     1
    // );

    xTaskCreatePinnedToCore(
        &RoasterControl::startHeaterSSR,
        "HeaterSSRTask",
        4096,
        this,
        1,
        NULL,
        1
    );
}

void RoasterControl::startFanSSR(void *param)
{
    RoasterControl* self = static_cast<RoasterControl*>(param);
    self->SSRTask(SSR_FAN_PIN, self->cycleFan_ms, self->fanLevel);
}

void RoasterControl::startHeaterSSR(void *param)
{
    RoasterControl* self = static_cast<RoasterControl*>(param);
    self->SSRTask(SSR_HEATER_PIN, self->cycleHeat_ms, self->heatLevel);
}

void RoasterControl::SSRTask(const uint8_t ssr_pin, const int32_t cycle_ms, volatile int32_t& level)
{
    while (true) {
        int current_level = level;

        int onTime = (cycle_ms * current_level) / 100;
        int offTime = cycle_ms - onTime;
        if (onTime > 0) {
            digitalWrite(ssr_pin, HIGH);
            Serial.printf("Pin %d on\n", ssr_pin);
            vTaskDelay(pdMS_TO_TICKS(onTime));
        }

        if (offTime > 0) {
            digitalWrite(ssr_pin, LOW);
            Serial.printf("Pin %d off\n", ssr_pin);
            vTaskDelay(pdMS_TO_TICKS(offTime));
        }
    }
}

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
    // Immedietly disable the heating element and turn on the fan
    digitalWrite(SSR_FAN_PIN, HIGH);
    digitalWrite(SSR_HEATER_PIN, LOW);

    Serial.printf("startCool. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::stopRoast()
{
    heatLevel = 0;
    fanLevel = 0;
    // Immedietly disable the fan and the heater
    digitalWrite(SSR_FAN_PIN, LOW);
    digitalWrite(SSR_HEATER_PIN, LOW);
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
    if (percentage > 0 && fanLevel < MIN_FAN_LEVEL_FOR_HEAT) {
        fanLevel = MIN_FAN_LEVEL_FOR_HEAT;
    }
    heatLevel = percentage;

    Serial.printf("Received heat percentage. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::setFan(int32_t percentage)
{
    if (percentage > MAX_LEVEL) {
        Serial.printf("Fan percentage too high: %d\n", percentage);
        return;
    }
    if (heatLevel > 0 && percentage < MIN_FAN_LEVEL_FOR_HEAT) {
        Serial.printf("Fan percentage too low, heat is still on: %d\n", percentage);
        return;
    }

    if (percentage < MIN_FAN_LEVEL) {
        Serial.printf("Fan percentage too low %d\n", percentage);
        return;
    }
    fanLevel = percentage;

    Serial.printf("Received fan percentage. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}