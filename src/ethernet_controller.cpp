#include <Arduino.h>
#include "ethernet_controller.h"
#include "config.h"
#include <SPIFFS.h>

// Global instance
EthernetController ethernetController;

// Configuration file path
const char* CONFIG_FILE = "/ethernet_config.json";

EthernetController::EthernetController() : 
    initialized(false),
    cs_pin(-1),
    rst_pin(-1)
{
    // Default MAC address
    mac[0] = 0xDE;
    mac[1] = 0xAD;
    mac[2] = 0xBE;
    mac[3] = 0xEF;
    mac[4] = 0xFE;
    mac[5] = 0xED;

    // Default network settings
    ip = IPAddress(192, 168, 1, 177);
    gateway = IPAddress(192, 168, 1, 1);
    subnet = IPAddress(255, 255, 255, 0);
    dns = IPAddress(192, 168, 1, 1);
}

EthernetController::~EthernetController() {
    // Nothing to clean up
}

bool EthernetController::begin(int cs_pin, int rst_pin) {
    Serial.println("[Ethernet] Initializing Ethernet controller...");
    
    this->cs_pin = cs_pin;
    this->rst_pin = rst_pin;
    
    // Try to load config from SPIFFS
    loadConfig();
    
    // Initialize reset pin
    pinMode(rst_pin, OUTPUT);
    digitalWrite(rst_pin, LOW);
    delay(10);
    digitalWrite(rst_pin, HIGH);
    delay(100);
    
    // Initialize Ethernet with CS pin
    Ethernet.init(cs_pin);

    // Set SPI clock divider
    SPI.setClockDivider(SPI_CLOCK_DIV8);  // Try SPI_CLOCK_DIV4 first
    
    // Start Ethernet connection
    Ethernet.begin(mac, ip, dns, gateway, subnet);
    
    // Check for hardware
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("[Ethernet] Shield was not found. Please check connections.");
        initialized = false;
        return false;
    }
    
    Serial.print("[Ethernet] Connected with IP: ");
    Serial.println(Ethernet.localIP());
    initialized = true;
    return true;
}

void EthernetController::loop() {
    if (!initialized) return;
    
    // Note: We're omitting the server functionality here since it seems to be causing issues
    // The WebServerManager will handle all HTTP requests
}

bool EthernetController::isConnected() const {
    if (!initialized) return false;
    return (Ethernet.linkStatus() == LinkON);
}

String EthernetController::getStatusJSON() {
    JsonDocument doc;
    
    doc["connected"] = isConnected();
    doc["ip"] = Ethernet.localIP().toString();
    doc["gateway"] = Ethernet.gatewayIP().toString();
    doc["subnet"] = Ethernet.subnetMask().toString();
    doc["dns"] = Ethernet.dnsServerIP().toString();
    
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

bool EthernetController::loadConfig() {
    if (!SPIFFS.exists(CONFIG_FILE)) {
        Serial.println("[Ethernet] Config file not found. Using defaults.");
        return saveConfig(); // Create default config file
    }
    
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (!configFile) {
        Serial.println("[Ethernet] Failed to open config file. Using defaults.");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();
    
    if (error) {
        Serial.print("[Ethernet] Failed to parse config file: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // Read MAC address
    if (doc.containsKey("mac") && doc["mac"].is<JsonArray>()) {
        JsonArray macArray = doc["mac"].as<JsonArray>();
        if (macArray.size() == 6) {
            for (int i = 0; i < 6; i++) {
                mac[i] = macArray[i];
            }
        }
    }
    
    // Read IP addresses
    if (doc.containsKey("ip")) {
        ip.fromString(doc["ip"].as<String>());
    }
    
    if (doc.containsKey("gateway")) {
        gateway.fromString(doc["gateway"].as<String>());
    }
    
    if (doc.containsKey("subnet")) {
        subnet.fromString(doc["subnet"].as<String>());
    }
    
    if (doc.containsKey("dns")) {
        dns.fromString(doc["dns"].as<String>());
    }
    
    Serial.println("[Ethernet] Configuration loaded from SPIFFS");
    return true;
}

bool EthernetController::saveConfig() {
    JsonDocument doc;
    
    // Create MAC address array
    JsonArray macArray = doc.createNestedArray("mac");
    for (int i = 0; i < 6; i++) {
        macArray.add(mac[i]);
    }
    
    // Add IP addresses
    doc["ip"] = ip.toString();
    doc["gateway"] = gateway.toString();
    doc["subnet"] = subnet.toString();
    doc["dns"] = dns.toString();
    
    // Open file for writing
    File configFile = SPIFFS.open(CONFIG_FILE, "w");
    if (!configFile) {
        Serial.println("[Ethernet] Failed to open config file for writing");
        return false;
    }
    
    // Write to file and close
    if (serializeJson(doc, configFile) == 0) {
        Serial.println("[Ethernet] Failed to write to config file");
        configFile.close();
        return false;
    }
    
    configFile.close();
    Serial.println("[Ethernet] Configuration saved to SPIFFS");
    return true;
}

bool EthernetController::updateConfig(IPAddress newIp, IPAddress newGateway, IPAddress newSubnet, IPAddress newDns) {
    Serial.println("[Ethernet] Updating configuration...");
    
    ip = newIp;
    gateway = newGateway;
    subnet = newSubnet;
    dns = newDns;
    
    return saveConfig();
}