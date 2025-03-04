#include <Arduino.h>
#include "neopixel_manager.h"

NeoPixelManager::NeoPixelManager() : 
    pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800),
    connectionState(false)
{
    redColor = pixels.Color(255, 0, 0);
    greenColor = pixels.Color(0, 255, 0);
}

void NeoPixelManager::begin() {
    pixels.begin();
    pixels.setBrightness(50); // Set to 50% brightness
    pixels.setPixelColor(0, redColor); // Start with red (no clients)
    pixels.show();
}

void NeoPixelManager::setConnectionState(bool connected) {
    // Only update if the state has changed
    if (connected != connectionState) {
        connectionState = connected;
        
        if (connectionState) {
            pixels.setPixelColor(0, greenColor);
        } else {
            pixels.setPixelColor(0, redColor);
        }
        
        pixels.show();
    }
}

bool NeoPixelManager::getConnectionState() const {
    return connectionState;
}