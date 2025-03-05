// webserver_manager.cpp
#include <Arduino.h>
#include "webserver_manager.h"
#include "ethernet_controller.h"
#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>

WebServerManager::WebServerManager(int port, DHTSensor *dhtSensor, DACControl *dacControl,
                                   I2CScanner *i2cScanner, SystemInfo *systemInfo,
                                   BatteryManager *batteryManager,
                                   EthernetController *ethernetController) : server(port),
                                                                     dhtSensor(dhtSensor),
                                                                     dacControl(dacControl),
                                                                     i2cScanner(i2cScanner),
                                                                     systemInfo(systemInfo),
                                                                     batteryManager(batteryManager),
                                                                     ethernetController(ethernetController)
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
    server.on("/api/ethernet/status", HTTP_GET, [this]()
              { this->handleEthernetStatus(); });
    server.on("/ethernet/config", HTTP_POST, [this]()
              { this->handleEthernetConfig(); });
    server.on("/ethernetModule.js", HTTP_GET, [this]()
              { this->handleJavaScriptFile("/ethernetModule.js"); });

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

void WebServerManager::handleEthernetStatus()
{
    Serial.println("[WebServer] Ethernet status requested");

    if (ethernetController != nullptr)
    {
        String statusJson = ethernetController->getStatusJSON();
        Serial.print("[WebServer] Ethernet status: ");
        Serial.println(statusJson);

        server.send(200, "application/json", statusJson);
    }
    else
    {
        server.send(503, "application/json", "{\"error\":\"Ethernet controller not available\"}");
    }
}

void WebServerManager::handleEthernetConfig()
{
    Serial.println("[WebServer] Ethernet configuration update requested");

    if (ethernetController == nullptr)
    {
        server.send(503, "application/json", "{\"success\":false,\"error\":\"Ethernet controller not available\"}");
        return;
    }

    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            String errorMsg = "{\"success\":false,\"error\":\"JSON parsing failed: ";
            errorMsg += error.c_str();
            errorMsg += "\"}";
            server.send(400, "application/json", errorMsg);
            return;
        }

        // Extract and validate IP addresses
        IPAddress newIp, newGateway, newSubnet, newDns;
        bool valid = true;

        if (doc.containsKey("ip"))
        {
            valid = valid && newIp.fromString(doc["ip"].as<String>());
        }
        else
        {
            newIp = ethernetController->getIP();
        }

        if (doc.containsKey("gateway"))
        {
            valid = valid && newGateway.fromString(doc["gateway"].as<String>());
        }
        else
        {
            newGateway = ethernetController->getGateway();
        }

        if (doc.containsKey("subnet"))
        {
            valid = valid && newSubnet.fromString(doc["subnet"].as<String>());
        }
        else
        {
            newSubnet = ethernetController->getSubnet();
        }

        if (doc.containsKey("dns"))
        {
            valid = valid && newDns.fromString(doc["dns"].as<String>());
        }
        else
        {
            newDns = ethernetController->getDns();
        }

        if (!valid)
        {
            server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid IP address format\"}");
            return;
        }

        // Update configuration
        if (ethernetController->updateConfig(newIp, newGateway, newSubnet, newDns))
        {
            server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuration saved. Please restart the device for changes to take effect.\"}");
        }
        else
        {
            server.send(500, "application/json", "{\"success\":false,\"error\":\"Failed to save configuration\"}");
        }
    }
    else
    {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing request body\"}");
    }
}

// Also modify the debug handler to include Ethernet routes
void WebServerManager::handleDebug()
{
    String output = "<html><body><h1>SPIFFS Debug Info</h1>";

    // List all files
    output += "<h2>Files in SPIFFS:</h2><ul>";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while (file)
    {
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
    output += "<li>/ethernetModule.js</li>";   // Add this new route
    output += "<li>/api/ethernet/status</li>"; // Add this new route
    output += "<li>/ethernet/config</li>";     // Add this new route
    output += "</ul>";

    // Also add Ethernet status to the debug info
    if (ethernetController != nullptr && ethernetController->isConnected())
    {
        output += "<h2>Ethernet Status:</h2>";
        output += "<p>Connected: Yes</p>";
        output += "<p>IP: " + ethernetController->getIP().toString() + "</p>";
        output += "<p>Gateway: " + ethernetController->getGateway().toString() + "</p>";
        output += "<p>Subnet: " + ethernetController->getSubnet().toString() + "</p>";
        output += "<p>DNS: " + ethernetController->getDns().toString() + "</p>";
    }
    else
    {
        output += "<h2>Ethernet Status:</h2>";
        output += "<p>Connected: No</p>";
    }

    // Show some general system info
    output += "<h2>System Info:</h2>";
    output += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
    output += "<p>ESP SDK: " + String(ESP.getSdkVersion()) + "</p>";
    output += "<p>Uptime: " + String(millis() / 1000) + " seconds</p>";
    output += "<p>Flash Size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB</p>";

    output += "</body></html>";

    server.send(200, "text/html", output);
}
