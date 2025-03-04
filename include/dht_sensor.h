#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>
#include "config.h"

class DHTSensor
{
private:
    DHT dht;
    unsigned long lastReadTime;
    bool readyFlag;
    float tempValue;
    float humidValue;

public:
    DHTSensor();
    void begin();
    void update();
    bool isReady() const;
    float getTemperature() const;
    float getHumidity() const;
};

#endif // DHT_SENSOR_H