#pragma once

#include <cstdint>

static constexpr int32_t MIN_FAN_LEVEL_FOR_HEAT = 50; // Minimum heat level is heat is on
static constexpr int32_t MIN_HEAT_LEVEL = 0;
static constexpr int32_t MIN_FAN_LEVEL = 0;
static constexpr int32_t MAX_LEVEL = 100;

class RoasterControl {
private:
    static void startFanSSR(void* param);
    static void startHeaterSSR(void* param);
    void SSRTask(const uint8_t ssr_pin, const int32_t cycle, volatile int32_t& level);

    volatile int32_t heatLevel = 0;
    volatile int32_t fanLevel = 0;
    const int32_t cycleFan_ms = 0;
    const int32_t cycleHeat_ms = 0;

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