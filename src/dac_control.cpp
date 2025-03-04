#include <Arduino.h>
#include "dac_control.h"
#include <driver/dac.h>


DACControl::DACControl() : value(0) {
}

void DACControl::begin() {
    // For ESP32-S2, enable the DAC channel (A0/GPIO18 corresponds to DAC_CHANNEL_1)
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, value); // Initialize DAC to 0
}

void DACControl::setValue(int newValue) {
    // Constrain value to valid range (0-255)
    value = constrain(newValue, 0, 255);
    
    // Update the DAC output
    dac_output_voltage(DAC_CHANNEL_1, value);
}

int DACControl::getValue() const {
    return value;
}