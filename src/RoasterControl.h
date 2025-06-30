#pragma once

#include <cstdint>

static constexpr int32_t MIN_FAN_LEVEL = 50;
static constexpr int32_t MIN_HEAT_LEVEL = 0;
static constexpr int32_t MAX_LEVEL = 100;

class RoasterControl {
private:
    int32_t heatLevel = 0;
    int32_t fanLevel = 0;
public:
    void startRoast();
    void startCool();
    void stopRoast();

    void setHeat(int32_t percentage);
    void setFan(int32_t percentage);
    
    int32_t getHeatLevel() const { return heatLevel; };
    int32_t getFanLevel() const { return fanLevel; };


};