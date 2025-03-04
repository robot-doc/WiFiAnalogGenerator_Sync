#include "config.h"

// Define the global variables that were declared as extern in config.h
const char* ap_ssid = "ESP32-S2-AP";
const char* ap_password = "password123";  // Minimum 8 characters

// IP Address details
const IPAddress local_ip(192, 168, 4, 1);
const IPAddress gateway(192, 168, 4, 1);
const IPAddress subnet(255, 255, 255, 0);