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
#define MUX1_OUT_GPIO        2     // ADC1_CHANNEL_2
#define MUX1_OUT_ADC         ADC1_CHANNEL_2

#define MUX2_OUT_GPIO        7     // ADC1_CHANNEL_6
#define MUX2_OUT_ADC         ADC1_CHANNEL_6

// ──────────────────────────────────────────────────────────────
//  MUX select lines
// ──────────────────────────────────────────────────────────────
#define MUX1_S0_GPIO 5
#define MUX1_S1_GPIO 6
#define MUX1_S2_GPIO 3
#define MUX1_S3_GPIO 4

#define MUX2_S0_GPIO 10
#define MUX2_S1_GPIO 11
#define MUX2_S2_GPIO 8
#define MUX2_S3_GPIO 9

//ADC INPUTS 
#define ADC_1 ADC1_CHANNEL_1
#define ADC_2 ADC1_CHANNEL_6

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



// list of pad primary sensor ADC channels, grouped by adc_type:  ie; mux 1, 2 or direct ADC"3"
// Array [adc_type][channel/gpio][input_num]
//input_num is the logical input number (1‑20), added for debugging purposes


//list of pad rim/secondary sensor ADC channels, grouped by adc_type:  ie; mux 1, 2 or direct ADC"3"
//input_num is the logical input number (1‑20), added for debugging purposes
//Array [adc_type][channel/gpio][input_num]

  
// ──────────────────────────────────────────────────────────────
//  Direct ADC mappings for inputs 17‑20
//  Each entry has up to two conductors (A & B).  If the second
//  conductor is unused, GPIO = 255.
// ──────────────────────────────────────────────────────────────
struct DirectADC {
    uint8_t gpio;
    union {
        adc1_channel_t adc1;
        adc2_channel_t adc2;
    } adc;
};

//GPIO, ADC_Channel
// 12 adc32 adc2_channel_1
// 13 adc33 adc2_channel_2
// 14 adc34 adc2_channel_3
// 15 adc35 adc2_channel_4
// 16 adc36 adc2_channel_5


//input number , <gpio>TIP , <gpio>RING
static const DirectADC directInputs[4][2] = {
    /* 17 */ { {14, {.adc2 = ADC2_CHANNEL_3}}, {12, {.adc2 = ADC2_CHANNEL_1}} },
    /* 18 */ { {13, {.adc2 = ADC2_CHANNEL_2}}, {MUX2_OUT_GPIO, {.adc1 = MUX2_OUT_ADC}} },
    /* 19 */ { {16, {.adc2 = ADC2_CHANNEL_5}}, {15, {.adc2 = ADC2_CHANNEL_4}} },
    /* 20 */ { {18, {.adc2 = ADC2_CHANNEL_2}}, {255, {.adc2 = ADC2_CHANNEL_MAX}} }
};

#endif  // ADC_INPUT_MAP_H
