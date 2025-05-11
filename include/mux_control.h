#ifndef MUX_CONTROL_H
#define MUX_CONTROL_H

#include <Arduino.h>
#include "adc_input_map.h"

// Select a channel on CD74HC4067 (mux = 1 or 2).
// ch: 0-15 selects one of 16 channels
// mux: 1 or 2 selects which multiplexer to control
void selectMuxChannel(uint8_t mux, uint8_t ch);

#endif // MUX_CONTROL_H 