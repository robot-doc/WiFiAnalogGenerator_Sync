#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials for the access point
extern const char *ap_ssid;
extern const char *ap_password;

// IP Address details
#include <IPAddress.h>
extern const IPAddress local_ip;
extern const IPAddress gateway;
extern const IPAddress subnet;

// Pin definitions
const int LED_PIN = 13;       // Built-in LED
const int DAC_PIN = 18;       // A0 on ESP32-S2 Feather = GPIO18
const int NEOPIXEL_PIN = 33;  // ESP32-S2 Feather built-in NeoPixel
const int NEOPIXEL_COUNT = 1; // Single NeoPixel
const int DHTPIN = 4;         // DHT11 sensor connected to GPIO4
#define DHTTYPE DHT11         // DHT 11 sensor type

// Timing constants
const unsigned long SENSOR_READ_INTERVAL = 2000; // Read sensor every 2 seconds

#endif // CONFIG_H