#pragma once

// Thermocouple pin assignments
#if defined(NODEMCU_V2)
#define SCK_PIN D5
#define SO_PIN D7
#define CS_PIN_BT D8
#define CS_PIN_ET D6
#elif defined(ESP32_C3)
#define SCK_PIN 4
#define SO_PIN 5
#define CS_PIN_BT 7
#define CS_PIN_ET 6
#elif defined(ESP32_DEV)
#define SCK_PIN SCK
#define SO_PIN MISO
#define CS_PIN_BT SS
#define CS_PIN_ET MOSI

#define DHT22_PIN GPIO_NUM_4
#define NTC_PIN GPIO_NUM_32
#define ZC_FAN_PIN GPIO_NUM_13
#define OUT_FAN_PIN GPIO_NUM_12
#define SSR_HEATER_PIN GPIO_NUM_27

#endif
