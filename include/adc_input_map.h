/*
 * adc_input_map.h
 * Schematic‑verified analog map for the D20 v1.1 drum module.
 */

#ifndef ADC_INPUT_MAP_H
#define ADC_INPUT_MAP_H

#include <driver/adc.h>

// ──────────────────────────────────────────────────────────────
//  Multiplexer outputs (wired to ESP32 ADC pins)
// ──────────────────────────────────────────────────────────────
#define MUX1_OUT_GPIO        7     // Physical GPIO pin for MUX1 output
#define MUX1_OUT_ADC         ADC1_CHANNEL_1  // ADC channel for MUX1

#define MUX2_OUT_GPIO        12    // Physical GPIO pin for MUX2 output
#define MUX2_OUT_ADC         ADC1_CHANNEL_6  // ADC channel for MUX2

// ──────────────────────────────────────────────────────────────
//  MUX select lines
// ──────────────────────────────────────────────────────────────
#define MUX1_S0_GPIO 10
#define MUX1_S1_GPIO 11
#define MUX1_S2_GPIO 8
#define MUX1_S3_GPIO 9

#define MUX2_S0_GPIO 15
#define MUX2_S1_GPIO 16
#define MUX2_S2_GPIO 13
#define MUX2_S3_GPIO 14

//Hihat switch inputs
#define HIHATSW1 ADC1_CHANNEL_0
#define HIHATSW2 ADC2_CHANNEL_6

// ──────────────────────────────────────────────────────────────
//  Channel‑to‑input lookup tables
//     mux1InputMap[ch] → logical input number (1‑20)
//     mux2InputMap[ch] → logical input number (1‑20)
// ──────────────────────────────────────────────────────────────
static const uint8_t mux1InputMap[16] = {
    6, 3, 4, 3, 4, 2, 1, 1, 10, 7, 8, 7, 8, 5, 6, 5
};

//input number mapped to the adc channel


static const uint8_t mux2InputMap[16] = {
    14, 11, 11, 12, 12, 9, 10, 9, 18, 15, 16, 15, 16, 13, 14, 13
};


// Direct ADC GPIO to channel mappings
// Format: GPIO -> ADC Channel
#define GPIO_12_ADC ADC2_CHANNEL_1  // 17 RING
#define GPIO_13_ADC ADC2_CHANNEL_2  // 18 TIP
#define GPIO_14_ADC ADC2_CHANNEL_3  // 17 TIP
#define GPIO_15_ADC ADC2_CHANNEL_4  // 19 RING
#define GPIO_16_ADC ADC2_CHANNEL_5  // 19 TIP
#define GPIO_18_ADC ADC2_CHANNEL_7  // 20 input (fixed from ADC2_CHANNEL_2)

#endif  // ADC_INPUT_MAP_H
