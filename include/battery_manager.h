// battery_manager.h
#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_LC709203F.h>
#include <ArduinoJson.h>

class BatteryManager {
private:
  Adafruit_LC709203F lc;
  float lastVoltage;
  unsigned long lastCheckTime;
  bool lastPowerState;
  bool monitorAvailable;

public:
  BatteryManager();
  bool begin();
  bool isConnected();
  bool isUSBPowered();
  bool isCharging();
  float getVoltage();
  float getPercentage();
  bool isMonitorAvailable() const;
  
  // Populate a JSON object with battery information
  void populateBatteryInfo(JsonObject& battery);
  
  // Get battery information as JSON
  String getBatteryInfoJSON();
};

#endif // BATTERY_MANAGER_H