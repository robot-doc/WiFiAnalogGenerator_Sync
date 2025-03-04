#ifndef NEOPIXEL_MANAGER_H
#define NEOPIXEL_MANAGER_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

class NeoPixelManager
{
private:
    Adafruit_NeoPixel pixels;
    bool connectionState;
    uint32_t redColor;
    uint32_t greenColor;

public:
    NeoPixelManager();
    void begin();
    void setConnectionState(bool connected);
    bool getConnectionState() const;
};

#endif // NEOPIXEL_MANAGER_H