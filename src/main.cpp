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
#include "soc/rtc_wdt.h"
#include "esp_task_wdt.h"

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
  // Disable the core 0 WDT
  disableCore0WDT();

  // Initialize serial communication
  Serial.begin(115200);
  delay(3000);

  // Check reset reason
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  switch (reason)
  {
  case ESP_RST_POWERON:
    Serial.println("Power-on reset");
    break;
  case ESP_RST_SW:
    Serial.println("Software reset");
    break;
  case ESP_RST_PANIC:
    Serial.println("Software panic reset");
    break;
  case ESP_RST_INT_WDT:
    Serial.println("Interrupt watchdog reset");
    break;
  case ESP_RST_TASK_WDT:
    Serial.println("Task watchdog reset");
    break;
  case ESP_RST_WDT:
    Serial.println("Other watchdog reset");
    break;
  default:
    Serial.printf("Unknown reset reason: %d\n", reason);
  }

  // Configure the watchdog
  esp_task_wdt_init(30, false); // 30 second timeout, no panic

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

  delay(100);

  // // Initialize Ethernet controller
  // Serial.println("Initializing Ethernet controller...");
  // if (ethernetController.begin(ETHERNET_CS_PIN, ETHERNET_RST_PIN))
  // {
  //   Serial.println("Ethernet controller initialized");
  // }
  // else
  // {
  //   Serial.println("Ethernet controller initialization failed. Check connections.");
  // }

  delay(100);
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

  // Feed the watchdog timer
  //esp_task_wdt_reset();

  static unsigned long lastHeapCheck = 0;
  if (millis() - lastHeapCheck > 5000)
  { // Every 5 seconds
    lastHeapCheck = millis();
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
  }

  // Small delay to prevent hogging the CPU
  delay(10);
}