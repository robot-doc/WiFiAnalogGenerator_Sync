#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <Arduino.h>
#include <Wire.h>
#include <vector>

class I2CScanner
{
private:
    bool scanComplete;
    std::vector<uint8_t> foundAddresses;

public:
    I2CScanner();
    void begin();
    void scan();
    const std::vector<uint8_t> &getFoundAddresses() const;
    bool isScanComplete() const;
    void clearScanResults();
    String getJSONResults() const;
};

#endif // I2C_SCANNER_H