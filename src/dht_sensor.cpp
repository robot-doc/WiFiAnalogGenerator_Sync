#include <Arduino.h>
#include "dht_sensor.h"


DHTSensor::DHTSensor() : 
    dht(DHTPIN, DHTTYPE), 
    lastReadTime(0), 
    readyFlag(false), 
    tempValue(0.0), 
    humidValue(0.0) 
{
}

void DHTSensor::begin() {
    dht.begin();
}

void DHTSensor::update() {
    unsigned long currentTime = millis();
    
    // Only read the sensor at the specified interval
    if (currentTime - lastReadTime < SENSOR_READ_INTERVAL) {
        return;
    }
    
    lastReadTime = currentTime;
    
    // Read humidity and temperature
    float newHumidity = dht.readHumidity();
    float newTemperature = dht.readTemperature();
    
    // Check if readings are valid
    if (!isnan(newHumidity) && !isnan(newTemperature)) {
        tempValue = newTemperature;
        humidValue = newHumidity;
        
        // Mark as ready if we have valid readings
        if (!readyFlag) {
            readyFlag = true;
        }
    }
}

bool DHTSensor::isReady() const {
    return readyFlag;
}

float DHTSensor::getTemperature() const {
    return tempValue;
}

float DHTSensor::getHumidity() const {
    return humidValue;
}