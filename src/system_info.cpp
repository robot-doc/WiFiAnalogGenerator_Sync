// system_info.cpp - with improved flash size formatting
#include "system_info.h"
#include <WiFi.h>
#include <ArduinoJson.h>

SystemInfo::SystemInfo(BatteryManager* batteryManager) : 
  batteryManager(batteryManager) 
{
}

String SystemInfo::getNetworkInfoJSON() {
  JsonDocument doc;
  
  if (WiFi.getMode() == WIFI_AP) {
    doc["mode"] = "Access Point";
    doc["ip"] = WiFi.softAPIP().toString();
    doc["ssid"] = WiFi.softAPSSID();
    doc["stations"] = WiFi.softAPgetStationNum();
  } else if (WiFi.getMode() == WIFI_STA) {
    doc["mode"] = "Station";
    doc["ip"] = WiFi.localIP().toString();
    doc["ssid"] = WiFi.SSID();
    doc["rssi"] = WiFi.RSSI();
  } else if (WiFi.getMode() == WIFI_AP_STA) {
    doc["mode"] = "AP+Station";
    doc["ip"] = WiFi.localIP().toString();
    doc["ap_ip"] = WiFi.softAPIP().toString();
  } else {
    doc["mode"] = "Disabled";
  }
  
  doc["mac"] = WiFi.macAddress();
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

String SystemInfo::getResourceInfoJSON() {
  JsonDocument doc;
  
  doc["cpuFreq"] = ESP.getCpuFreqMHz();
  doc["freeHeap"] = ESP.getFreeHeap() / 1024.0;  // Convert to KB
  doc["totalHeap"] = ESP.getHeapSize() / 1024.0; // Convert to KB
  
  // Use heap information as RAM info
  doc["freeRam"] = ESP.getFreeHeap() / 1024.0;  // Convert to KB
  doc["totalRam"] = ESP.getHeapSize() / 1024.0; // Convert to KB
  
  // Flash information - Fix units to show MB instead of KB
  float flashSizeMB = ESP.getFlashChipSize() / (1024.0 * 1024.0);  // Convert to MB
  doc["flashSize"] = flashSizeMB;
  doc["flashSizeUnits"] = "MB";  // Add units explicitly
  doc["flashSpeed"] = ESP.getFlashChipSpeed() / 1000000.0; // Convert to MHz
  
  // Get sketch information to estimate flash usage
  float sketchSizeKB = ESP.getSketchSize() / 1024.0;
  float freeSpaceKB = ESP.getFreeSketchSpace() / 1024.0;
  
  // Convert to MB if large enough
  if (sketchSizeKB > 1024) {
    doc["sketchSize"] = sketchSizeKB / 1024.0;
    doc["sketchSizeUnits"] = "MB";
  } else {
    doc["sketchSize"] = sketchSizeKB;
    doc["sketchSizeUnits"] = "KB";
  }
  
  if (freeSpaceKB > 1024) {
    doc["freeSketchSpace"] = freeSpaceKB / 1024.0;
    doc["freeSketchSpaceUnits"] = "MB";
  } else {
    doc["freeSketchSpace"] = freeSpaceKB;
    doc["freeSketchSpaceUnits"] = "KB";
  }
  
  // Flash usage percentage
  float flashUsedPercent = (float)ESP.getSketchSize() * 100.0 / ESP.getFlashChipSize();
  doc["flashUsedPercent"] = flashUsedPercent;
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

String SystemInfo::getBoardInfoJSON() {
  JsonDocument doc;
  
  doc["chipModel"] = "ESP32-S2";
  doc["chipRevision"] = ESP.getChipRevision();
  doc["sdkVersion"] = ESP.getSdkVersion();
  
  // Calculate uptime
  unsigned long uptime = millis() / 1000; // Convert to seconds
  doc["uptime"] = uptime;
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

String SystemInfo::getCompleteSystemInfoJSON() {
  JsonDocument doc;
  
  // Create nested objects and populate them directly
  JsonObject network = doc.createNestedObject("network");
  populateNetworkInfo(network);
  
  JsonObject resources = doc.createNestedObject("resources");
  populateResourceInfo(resources);
  
  JsonObject board = doc.createNestedObject("board");
  populateBoardInfo(board);
  
  // Add battery info if available - SIMPLIFIED APPROACH
  if (batteryManager) {
    // Get battery info as JSON string
    String batteryJson = batteryManager->getBatteryInfoJSON();
    
    // Parse the JSON string into a temporary document
    JsonDocument tempDoc;
    deserializeJson(tempDoc, batteryJson);
    
    // Create the battery object in main document and copy values
    JsonObject battery = doc.createNestedObject("battery");
    for (JsonPair kv : tempDoc.as<JsonObject>()) {
      battery[kv.key()] = kv.value();
    }
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

void SystemInfo::populateNetworkInfo(JsonObject& network) {
  if (WiFi.getMode() == WIFI_AP) {
    network["mode"] = "Access Point";
    network["ip"] = WiFi.softAPIP().toString();
    network["ssid"] = WiFi.softAPSSID();
    network["stations"] = WiFi.softAPgetStationNum();
  } else if (WiFi.getMode() == WIFI_STA) {
    network["mode"] = "Station";
    network["ip"] = WiFi.localIP().toString();
    network["ssid"] = WiFi.SSID();
    network["rssi"] = WiFi.RSSI();
  } else if (WiFi.getMode() == WIFI_AP_STA) {
    network["mode"] = "AP+Station";
    network["ip"] = WiFi.localIP().toString();
    network["ap_ip"] = WiFi.softAPIP().toString();
  } else {
    network["mode"] = "Disabled";
  }
  
  network["mac"] = WiFi.macAddress();
}

void SystemInfo::populateResourceInfo(JsonObject& resources) {
  resources["cpuFreq"] = ESP.getCpuFreqMHz();
  resources["freeHeap"] = ESP.getFreeHeap() / 1024.0;  // Convert to KB
  resources["totalHeap"] = ESP.getHeapSize() / 1024.0; // Convert to KB
  
  // Use heap information as RAM info
  resources["freeRam"] = ESP.getFreeHeap() / 1024.0;  // Convert to KB
  resources["totalRam"] = ESP.getHeapSize() / 1024.0; // Convert to KB
  
  // Flash information - Fix units to show MB instead of KB
  float flashSizeMB = ESP.getFlashChipSize() / (1024.0 * 1024.0);  // Convert to MB
  resources["flashSize"] = flashSizeMB;
  resources["flashSizeUnits"] = "MB";  // Add units explicitly
  resources["flashSpeed"] = ESP.getFlashChipSpeed() / 1000000.0; // Convert to MHz
  
  // Get sketch information to estimate flash usage
  float sketchSizeKB = ESP.getSketchSize() / 1024.0;
  float freeSpaceKB = ESP.getFreeSketchSpace() / 1024.0;
  
  // Convert to MB if large enough
  if (sketchSizeKB > 1024) {
    resources["sketchSize"] = sketchSizeKB / 1024.0;
    resources["sketchSizeUnits"] = "MB";
  } else {
    resources["sketchSize"] = sketchSizeKB;
    resources["sketchSizeUnits"] = "KB";
  }
  
  if (freeSpaceKB > 1024) {
    resources["freeSketchSpace"] = freeSpaceKB / 1024.0;
    resources["freeSketchSpaceUnits"] = "MB";
  } else {
    resources["freeSketchSpace"] = freeSpaceKB;
    resources["freeSketchSpaceUnits"] = "KB";
  }
  
  // Flash usage percentage
  float flashUsedPercent = (float)ESP.getSketchSize() * 100.0 / ESP.getFlashChipSize();
  resources["flashUsedPercent"] = flashUsedPercent;
}

void SystemInfo::populateBoardInfo(JsonObject& board) {
  board["chipModel"] = "ESP32-S2";
  board["chipRevision"] = ESP.getChipRevision();
  board["sdkVersion"] = ESP.getSdkVersion();
  
  // Calculate uptime
  unsigned long uptime = millis() / 1000; // Convert to seconds
  board["uptime"] = uptime;
}