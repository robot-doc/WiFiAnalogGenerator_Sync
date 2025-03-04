#ifndef DAC_CONTROL_H
#define DAC_CONTROL_H

#include "config.h"

class DACControl
{
private:
    int value;

public:
    DACControl();
    void begin();
    void setValue(int newValue);
    int getValue() const;
};

#endif // DAC_CONTROL_H