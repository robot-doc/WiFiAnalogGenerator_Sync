#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h>
#include <WebServer.h> // Make sure this include is present
#include <SPIFFS.h>
#include "dht_sensor.h"
#include "dac_control.h"
#include "i2c_scanner.h"
#include "system_info.h"
#include "battery_manager.h"

class WebServerManager
{
private:
    WebServer server;
    DHTSensor *dhtSensor;
    DACControl *dacControl;
    I2CScanner *i2cScanner;
    SystemInfo *systemInfo;
    BatteryManager *batteryManager;

    // Private handler methods
    void handleRoot();
    void handleCSS();
    void handleJavaScriptFile(const String &filename);
    void handleLED();
    void handleLEDState();
    void handleDAC();
    void handleDACState();
    void handleClients();
    void handleSensor();
    void handleScan();
    void handleSystemInfo();
    void handleDebug();

    // Helper method to serve files
    void serveFile(const String &path, const String &contentType);

public:
    WebServerManager(int port, DHTSensor *dhtSensor, DACControl *dacControl,
                     I2CScanner *i2cScanner, SystemInfo *systemInfo,
                     BatteryManager *batteryManager);
    void begin();
    void handleClient();
};

#endif // WEBSERVER_MANAGER_H