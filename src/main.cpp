#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPIFFS.h>
#include "config.h"
#include "dht_sensor.h"
#include "dac_control.h"
#include "neopixel_manager.h"
#include "i2c_scanner.h"
#include "battery_manager.h"
#include "system_info.h"
#include "webserver_manager.h"
#include <SPI.h>
#include "ethernet_controller.h"

// Define Ethernet pins - add these to your user_config.h if you have one,
// or define them here if not
#define ETHERNET_CS_PIN 33
#define ETHERNET_RST_PIN 32

// Create objects
DHTSensor dhtSensor;
DACControl dacControl;
NeoPixelManager neoPixel;
I2CScanner i2cScanner;
BatteryManager batteryManager;
SystemInfo systemInfo(&batteryManager);
WebServerManager webServer(80, &dhtSensor, &dacControl, &i2cScanner, &systemInfo, &batteryManager, &ethernetController);

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32-S2 Feather starting up...");

  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Start with LED off

  // Initialize I2C
  Wire.begin();

  // Initialize components
  batteryManager.begin();
  dacControl.begin();
  neoPixel.begin();
  dhtSensor.begin();
  i2cScanner.begin();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // List files in SPIFFS for debugging
  Serial.println("Files found in SPIFFS:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    Serial.print("  ");
    Serial.println(file.name());
    file = root.openNextFile();
  }

  // Configure access point
  WiFi.softAP(ap_ssid, ap_password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.print("Access Point IP address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize Ethernet controller
  Serial.println("Initializing Ethernet controller...");
  if (ethernetController.begin(ETHERNET_CS_PIN, ETHERNET_RST_PIN))
  {
    Serial.println("Ethernet controller initialized");
  }
  else
  {
    Serial.println("Ethernet controller initialization failed. Check connections.");
  }

  // Initialize and start the web server
  webServer.begin();
}

void loop()
{
  // Handle client requests
  webServer.handleClient();

  // Update sensor readings
  dhtSensor.update();

  // Check client connections and update NeoPixel accordingly
  int clientCount = WiFi.softAPgetStationNum();
  neoPixel.setConnectionState(clientCount > 0);

  // Process Ethernet client requests
  ethernetController.loop();

  // Small delay to prevent hogging the CPU
  delay(10);
}