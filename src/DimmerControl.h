#pragma once

#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


const int MIN_US_PULSE = 50;  // Minimale Pulsbreite für den Triac-Trigger
const int HALF_CYCLE_US = 10000; // 10ms für eine halbe 50Hz-Periode


const int powerLevels[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
const int delayTimesUs[] = {
  9900, // 0% Leistung (fast volle Verzögerung)
  8000, // 5%
  7400, // 10%
  7000, // 15%
  6600, // 20%
  6300, // 25%
  6000, // 30%
  5750, // 35%
  5500, // 40%
  5250, // 45%
  5000, // 50%
  4750, // 55%
  4500, // 60%
  4250, // 65%
  4000, // 70%
  3750, // 75%
  3300, // 80%
  3000, // 85%
  2500, // 90%
  2000, // 95%
  500, // 100%, some delay
};

const int NUM_LUT_ENTRIES = sizeof(powerLevels) / sizeof(powerLevels[0]);

class DimmerControl
{
public:
    DimmerControl(uint8_t triac_pin, uint8_t zc_pin);
    ~DimmerControl();
    void begin();
    void setPowerLevel(uint8_t level);
    void powerOff();
    void powerOn();
private:
    static void startDimmerTask(void* param);

    void IRAM_ATTR zeroCrossISR();
    void dimmerControl();

    TaskHandle_t dimmerTaskHandle = nullptr;

    const uint8_t zc_pin;
    const uint8_t triac_pin;

    volatile int currentDealy = 0;
    volatile bool enabled = false;
    volatile bool zc_detected = false;
};

