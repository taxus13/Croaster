#include "RoasterControl.h"

#include "HardwareSerial.h"
#include "PinConfig.h"


RoasterControl::RoasterControl(int32_t cycleHeat_ms) : cycleHeat_ms(cycleHeat_ms), dimmer(OUT_FAN_PIN, ZC_FAN_PIN)
{
}

void RoasterControl::begin()
{
    pinMode(SSR_HEATER_PIN, OUTPUT);
    digitalWrite(SSR_HEATER_PIN, LOW);
    dimmer.begin();

}

void RoasterControl::startHeaterSSR(void *param)
{
    RoasterControl* self = static_cast<RoasterControl*>(param);
    self->SSRTaskHeater(SSR_HEATER_PIN, self->cycleHeat_ms, self->heatLevel);
}

void RoasterControl::SSRTaskHeater(const uint8_t ssr_pin, const int32_t cycle_ms, volatile int32_t& level)
{
    const int sleep_duration = pdMS_TO_TICKS(100);
    int last_level = 0;

    while (true) {
        {
            last_level = level;
            int onTime = pdMS_TO_TICKS((cycle_ms * last_level) / 100);
            Serial.printf("level: %d onTime: %d\n",last_level, onTime);
            if (onTime > 0) {
                digitalWrite(ssr_pin, HIGH);
                // Serial.printf("Pin %d on\n", ssr_pin);
                for (int total_sleep_time = 0; total_sleep_time < onTime; total_sleep_time += sleep_duration) {
                    vTaskDelay(sleep_duration);
                    if (last_level != level) {
                        Serial.printf("Level changed from %d to %d (was in on state)\n", last_level, level);
                        break;
                    }
                }
            }
        }
        {
            last_level = level;
            int offTime  = (cycle_ms * (100 - last_level)) / 100;
            Serial.printf("level: %d offTime: %d\n",last_level, offTime);
            if (offTime > 0) {
                digitalWrite(ssr_pin, LOW);
                // Serial.printf("Pin %d off\n", ssr_pin);
                for (int total_sleep_time = 0; total_sleep_time < offTime; total_sleep_time += sleep_duration) {
                    vTaskDelay(sleep_duration);
                    if (last_level != level) {
                        Serial.printf("Level changed from %d to %d (was in off state)\n", last_level, level);
                        break;
                    }
                }
            }
        }
    }
}

void RoasterControl::stopTask(TaskHandle_t &handle)
{
    if (handle != nullptr) {
        vTaskDelete(handle);
        handle = nullptr;
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
    dimmer.setPowerLevel(MAX_LEVEL);
    Serial.printf("startCool. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}

void RoasterControl::stopRoast()
{
    heatLevel = 0;
    fanLevel = 0;
    // Immedietly disable the fan and the heater
    // stopTask(heatTaskHandle);
    // stopTask(fanTaskHandle);

    digitalWrite(SSR_HEATER_PIN, LOW);
    dimmer.powerOff();
    dimmer.setPowerLevel(0);

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
        Serial.println("Heat turned on without fan, turning it on");
        setFan(MIN_FAN_LEVEL_FOR_HEAT);
    }
    heatLevel = percentage;

    if (heatTaskHandle == nullptr) {
        Serial.println("Heat task not running, starting...");
        xTaskCreatePinnedToCore(
            &RoasterControl::startHeaterSSR,
            "HeaterSSRTask",
            4096,
            this,
            1,
            &heatTaskHandle,
            1
        );
    }

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
    if (fanLevel == 0) {
        dimmer.powerOff();
    } else {
        dimmer.powerOn();
        dimmer.setPowerLevel(fanLevel);
    }

    Serial.printf("Received fan percentage. Current values H=%d,F=%d\n", heatLevel, fanLevel);
}