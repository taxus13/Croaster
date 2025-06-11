#pragma once

#include "DHT.h"

class DHTHandler {
private:
    DHT dht;
    unsigned long lastSensorRead = 0;
    float temperature = 0;
    float humidity = 0;
public:
    DHTHandler();
    float getTemperature() const;
    float getHumidity() const;
    void begin();
    void loop();

};