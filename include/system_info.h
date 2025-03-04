// system_info.h
#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "battery_manager.h"

class SystemInfo {
private:
  BatteryManager* batteryManager;

public:
  SystemInfo(BatteryManager* batteryManager);
  
  // Get network information as JSON
  String getNetworkInfoJSON();
  
  // Get resource information as JSON
  String getResourceInfoJSON();
  
  // Get board information as JSON
  String getBoardInfoJSON();
  
  // Get complete system information as JSON
  String getCompleteSystemInfoJSON();
  
  // Populate JSON objects directly
  void populateNetworkInfo(JsonObject& network);
  void populateResourceInfo(JsonObject& resources);
  void populateBoardInfo(JsonObject& board);
};

#endif // SYSTEM_INFO_H