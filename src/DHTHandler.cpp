#include "DHTHandler.h"

DHTHandler::DHTHandler(uint8_t pin) : dht(pin, DHT22) {

}

float DHTHandler::getTemperature() const {
    return temperature;
}

float DHTHandler::getHumidity() const {
    return humidity;
}


void DHTHandler::begin()
{
    dht.begin();
}

void DHTHandler::loop() {
    unsigned long now = millis();

    if (now - lastSensorRead < 2000) {
        return;
    }
    lastSensorRead = now;

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
}