#pragma once

#include <cstdint>

static constexpr MIN_FAN_LEVEL = 50;
static constexpr MIN_HEAT_LEVEL = 50;
static constexpr MAX_LEVEL = 100;

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


};