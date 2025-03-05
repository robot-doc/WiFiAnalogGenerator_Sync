// battery_manager.cpp
#include <Arduino.h>
#include "battery_manager.h"


BatteryManager::BatteryManager() : 
  lastVoltage(0),
  lastCheckTime(0),
  lastPowerState(false),
  monitorAvailable(false)
{
}

bool BatteryManager::begin() {
  // Try to initialize the LC709203F battery monitor
  if (!lc.begin()) {
    Serial.println("Couldn't find LC709203F battery monitor");
    monitorAvailable = false;
    return false;
  }
  
  Serial.println("Found LC709203F battery monitor");
  monitorAvailable = true;
  
  // Set up the LC709203F
  lc.setPackSize(LC709203F_APA_500MAH);  // Adjust this to match your battery capacity
  lc.setAlarmVoltage(3.8);  // Set low voltage alarm
  
  return true;
}

bool BatteryManager::isConnected() {
  if (!monitorAvailable) return false;
  
  // Get the voltage from the LC709203F
  float cellVoltage = lc.cellVoltage();
  
  // Get the percentage - an additional check
  float cellPercent = lc.cellPercent();
  
  // Print debug info to serial
  Serial.print("Battery voltage: ");
  Serial.print(cellVoltage);
  Serial.print("V, percentage: ");
  Serial.print(cellPercent);
  Serial.println("%");
  
  // For the ESP32-S2 Feather specifically:
  // 1. If voltage is below 2.5V, almost certainly no battery
  // 2. If voltage is above 2.5V, likely a battery is connected
  // 3. In rare cases, the LC709203F might report a voltage even without a battery
  //    so we also check if the percentage is at least 1%
  return (cellVoltage >= 2.5) && (cellPercent > 0.0);
}

bool BatteryManager::isUSBPowered() {
  if (!monitorAvailable) return true;
  
  // Try to detect if USB is connected
  
  // Method 1: Check if VBUS is present on the USB connector
  #if defined(PIN_USB_DETECT) 
  pinMode(PIN_USB_DETECT, INPUT);
  if (digitalRead(PIN_USB_DETECT) == HIGH) {
    return true;
  }
  #endif
  
  // Method 2: If we know the device is working and a battery is connected,
  // we can infer power source from the battery voltage behavior:
  // - If voltage remains constant at ~4.2V, likely USB powered & charged
  // - If voltage is slowly decreasing, likely on battery power
  float currentVoltage = lc.cellVoltage();
  unsigned long currentTime = millis();
  
  // Only update our decision if enough time has passed
  if (currentTime - lastCheckTime > 30000) { // Check every 30 seconds
    if (currentVoltage > 4.1 && abs(currentVoltage - lastVoltage) < 0.05) {
      // High stable voltage indicates charging or charged via USB
      lastPowerState = true;
    } else if (currentVoltage < lastVoltage - 0.02) {
      // Decreasing voltage indicates battery power
      lastPowerState = false;
    }
    
    lastVoltage = currentVoltage;
    lastCheckTime = currentTime;
  }
  
  return lastPowerState;
}

bool BatteryManager::isCharging() {
  if (!monitorAvailable || !isConnected()) return false;
  
  // If USB powered and voltage is below 4.2V, it's likely charging
  return isUSBPowered() && (lc.cellVoltage() < 4.2);
}

float BatteryManager::getVoltage() {
  if (!monitorAvailable || !isConnected()) return 0.0;
  return lc.cellVoltage();
}

float BatteryManager::getPercentage() {
  if (!monitorAvailable || !isConnected()) return 0.0;
  return lc.cellPercent();
}

bool BatteryManager::isMonitorAvailable() const {
  return monitorAvailable;
}

void BatteryManager::populateBatteryInfo(JsonObject& battery) {
  bool batteryConnected = isConnected();
  battery["connected"] = batteryConnected;
  
  if (batteryConnected) {
    battery["voltage"] = getVoltage();
    battery["percentage"] = getPercentage();
    battery["usbPowered"] = isUSBPowered();
    battery["charging"] = isCharging();
  } else {
    battery["usbPowered"] = true; // If no battery, must be USB powered
  }
}

String BatteryManager::getBatteryInfoJSON() {
  JsonDocument doc;
  
  bool batteryConnected = isConnected();
  doc["connected"] = batteryConnected;
  
  if (batteryConnected) {
    doc["voltage"] = getVoltage();
    doc["percentage"] = getPercentage();
    doc["usbPowered"] = isUSBPowered();
    doc["charging"] = isCharging();
  } else {
    doc["usbPowered"] = true; // If no battery, must be USB powered
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}