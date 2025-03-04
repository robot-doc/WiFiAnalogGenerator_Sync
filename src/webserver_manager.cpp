// webserver_manager.cpp
#include "webserver_manager.h"
#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>

WebServerManager::WebServerManager(int port, DHTSensor *dhtSensor, DACControl *dacControl,
                                   I2CScanner *i2cScanner, SystemInfo *systemInfo,
                                   BatteryManager *batteryManager) : server(port),
                                                                     dhtSensor(dhtSensor),
                                                                     dacControl(dacControl),
                                                                     i2cScanner(i2cScanner),
                                                                     systemInfo(systemInfo),
                                                                     batteryManager(batteryManager)
{
    // Constructor body can be empty or have initialization code
}

void WebServerManager::begin()
{
    // Set up all routes
    server.on("/", HTTP_GET, [this]()
              { this->handleRoot(); });
    server.on("/style.css", HTTP_GET, [this]()
              { this->handleCSS(); });

    // JavaScript module routes
    server.on("/controlModule.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/controlModule.js"); });
    server.on("/scannerModule.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/scannerModule.js"); });
    server.on("/sysInfoModule.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/sysInfoModule.js"); });
    server.on("/tabModule.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/tabModule.js"); });
    server.on("/main.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/main.js"); });

    // API routes
    server.on("/led", HTTP_GET, [this]()
              { this->handleLED(); });
    server.on("/ledstate", HTTP_GET, [this]()
              { this->handleLEDState(); });
    server.on("/dac", HTTP_GET, [this]()
              { this->handleDAC(); });
    server.on("/dacstate", HTTP_GET, [this]()
              { this->handleDACState(); });
    server.on("/clients", HTTP_GET, [this]()
              { this->handleClients(); });
    server.on("/sensor", HTTP_GET, [this]()
              { this->handleSensor(); });
    server.on("/scan", HTTP_GET, [this]()
              { this->handleScan(); });
    server.on("/sysinfo", HTTP_GET, [this]()
              { this->handleSystemInfo(); });
    server.on("/debug", HTTP_GET, [this]()
              { this->handleDebug(); });

    // Start the server
    server.begin();
    Serial.println("Web server started");
}

void WebServerManager::handleClient()
{
    server.handleClient();
}

// Helper function to serve files from SPIFFS
void WebServerManager::serveFile(const String &path, const String &contentType)
{
    if (SPIFFS.exists(path))
    {
        File file = SPIFFS.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
    }
    else
    {
        server.send(404, "text/plain", "File not found: " + path);
        Serial.println("File not found: " + path);
    }
}

// Route handlers
void WebServerManager::handleRoot()
{
    serveFile("/index.html", "text/html");
}

void WebServerManager::handleCSS()
{
    serveFile("/style.css", "text/css");
}

void WebServerManager::handleJavaScriptFile(const String &filename)
{
    serveFile(filename, "application/javascript");
}

void WebServerManager::handleLED()
{
    String state;
    if (server.hasArg("state"))
    {
        state = server.arg("state");
        digitalWrite(LED_PIN, state.toInt());
        Serial.print("[WebServer] LED state set to: ");
        Serial.println(state);
    }
    server.send(200, "text/plain", "LED state set to " + state);
}

void WebServerManager::handleLEDState()
{
    String state = String(digitalRead(LED_PIN));
    Serial.print("[WebServer] LED state requested: ");
    Serial.println(state);
    server.send(200, "text/plain", state);
}

void WebServerManager::handleDAC()
{
    if (server.hasArg("value"))
    {
        int value = server.arg("value").toInt();
        dacControl->setValue(value);
        Serial.print("[WebServer] DAC value set to: ");
        Serial.println(value);
    }
    server.send(200, "text/plain", String(dacControl->getValue()));
}

void WebServerManager::handleDACState()
{
    int value = dacControl->getValue();
    Serial.print("[WebServer] DAC state requested: ");
    Serial.println(value);
    server.send(200, "text/plain", String(value));
}

void WebServerManager::handleClients()
{
    int clients = WiFi.softAPgetStationNum();
    Serial.print("[WebServer] Client count requested: ");
    Serial.println(clients);
    server.send(200, "text/plain", String(clients));
}

void WebServerManager::handleSensor()
{
    // Create a JSON response with sensor data
    String sensorJson = "{";
    sensorJson += "\"ready\":" + String(dhtSensor->isReady() ? "true" : "false") + ",";
    sensorJson += "\"temperature\":" + String(dhtSensor->getTemperature()) + ",";
    sensorJson += "\"humidity\":" + String(dhtSensor->getHumidity());
    sensorJson += "}";

    Serial.print("[WebServer] Sensor data requested: ");
    Serial.println(sensorJson);

    server.send(200, "application/json", sensorJson);
}

void WebServerManager::handleScan()
{
    Serial.println("[WebServer] Scan request received");

    // Perform the scan directly
    i2cScanner->scan();

    // Get and return results
    String results = i2cScanner->getJSONResults();
    Serial.print("[WebServer] Scan results: ");
    Serial.println(results);

    server.send(200, "application/json", results);
}

void WebServerManager::handleSystemInfo()
{
    Serial.println("System Info requested");

    // Get system info JSON from the SystemInfo class
    String jsonResponse = systemInfo->getCompleteSystemInfoJSON();

    // Send the response
    server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleDebug() {
    String output = "<html><body><h1>SPIFFS Debug Info</h1>";
    
    // List all files
    output += "<h2>Files in SPIFFS:</h2><ul>";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    
    while (file) {
      output += "<li>" + String(file.name()) + " (" + String(file.size()) + " bytes)</li>";
      file = root.openNextFile();
    }
    output += "</ul>";
    
    // List registered server routes
    output += "<h2>Web Server Routes:</h2><ul>";
    output += "<li>/</li>";
    output += "<li>/style.css</li>";
    output += "<li>/led</li>";
    output += "<li>/ledstate</li>";
    output += "<li>/dac</li>";
    output += "<li>/dacstate</li>";
    output += "<li>/clients</li>";
    output += "<li>/sensor</li>";
    output += "<li>/scan</li>";
    output += "<li>/sysinfo</li>";
    output += "<li>/debug</li>";
    output += "<li>/controlModule.js</li>";
    output += "<li>/scannerModule.js</li>";
    output += "<li>/sysInfoModule.js</li>";
    output += "<li>/tabModule.js</li>";
    output += "<li>/main.js</li>";
    output += "</ul>";
    
    // Show some general system info
    output += "<h2>System Info:</h2>";
    output += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
    output += "<p>ESP SDK: " + String(ESP.getSdkVersion()) + "</p>";
    output += "<p>Uptime: " + String(millis() / 1000) + " seconds</p>";
    output += "<p>Flash Size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB</p>";
    
    output += "</body></html>";
    
    server.send(200, "text/html", output);
  }