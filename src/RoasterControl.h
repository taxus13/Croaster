#pragma once

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr int32_t MIN_FAN_LEVEL_FOR_HEAT = 30; // Minimum heat level is heat is on
static constexpr int32_t MIN_HEAT_LEVEL = 0;
static constexpr int32_t MIN_FAN_LEVEL = 0;
static constexpr int32_t MAX_LEVEL = 100;

class RoasterControl {
private:
    static void startFanSSR(void* param);
    static void startHeaterSSR(void* param);
    void SSRTaskFan(const uint8_t ssr_pin, const int32_t cycle, volatile int32_t& level);
    void SSRTaskHeater(const uint8_t ssr_pin, const int32_t cycle, volatile int32_t& level);
    void stopTask(TaskHandle_t& handle);

    volatile int32_t heatLevel = 0;
    volatile int32_t fanLevel = 0;
    const int32_t cycleFan_ms = 0;
    const int32_t cycleHeat_ms = 0;


    TaskHandle_t heatTaskHandle = nullptr;
    TaskHandle_t fanTaskHandle = nullptr;
public:
    RoasterControl(int32_t cycleFan_ms, int32_t cycleHeat_ms);
    void begin();

    void startRoast();
    void startCool();
    void stopRoast();

    void setHeat(int32_t percentage);
    void setFan(int32_t percentage);
    
    int32_t getHeatLevel() const { return heatLevel; };
    int32_t getFanLevel() const { return fanLevel; };
};