#include <Arduino.h>
#include "i2c_scanner.h"

I2CScanner::I2CScanner() : scanComplete(false)
{
}

void I2CScanner::begin()
{
    // I2C is already initialized in main.cpp
    Serial.println("[I2C Scanner] Initialized");
}

// Modified i2c_scanner.cpp for better reliability
void I2CScanner::scan()
{
    Serial.println("[I2C Scanner] Starting scan...");

    // Clear previous results
    foundAddresses.clear();
    scanComplete = false;

    // Save current I2C settings
    uint32_t originalClock = Wire.getClock();
    
    // Reset I2C
    Wire.end();
    delay(100);
    Wire.begin();
    
    // Use 100kHz clock - same as most Arduino examples
    Wire.setClock(100000);

    byte error, address;
    int deviceCount = 0;

    // Use exact approach from standard Arduino scanner
    for(address = 1; address < 127; address++)
    {
        Serial.println(address); // Keep this for debugging
        
        // Add a small delay to give time for serial output
        delay(5);
        
        // The i2c_scanner uses the return value of
        // the Write.endTransmission to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("[I2C Scanner] Device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            
            foundAddresses.push_back(address);
            deviceCount++;
        }
    }
    
    // Restore original I2C settings
    Wire.end();
    delay(50);
    Wire.begin();
    Wire.setClock(originalClock);
    
    Serial.print("[I2C Scanner] Scan complete. Found ");
    Serial.print(deviceCount);
    Serial.println(" devices.");

    scanComplete = true;
}

const std::vector<uint8_t> &I2CScanner::getFoundAddresses() const
{
    return foundAddresses;
}

bool I2CScanner::isScanComplete() const
{
    return scanComplete;
}

void I2CScanner::clearScanResults()
{
    foundAddresses.clear();
    scanComplete = false;
    Serial.println("[I2C Scanner] Results cleared");
}

String I2CScanner::getJSONResults() const
{
    String json = "{\"scanComplete\":" + String(scanComplete ? "true" : "false") + ",";
    json += "\"devices\":[";

    if (scanComplete)
    {
        for (size_t i = 0; i < foundAddresses.size(); i++)
        {
            if (i > 0)
            {
                json += ",";
            }

            uint8_t address = foundAddresses[i];
            json += "{";
            json += "\"address\":" + String(address) + ",";
            json += "\"hexAddress\":\"0x";
            if (address < 16) json += "0"; // Ensure two-digit hex format
            json += String(address, HEX) + "\"";
            json += "}";
        }
    }

    json += "]}";

    return json;
}