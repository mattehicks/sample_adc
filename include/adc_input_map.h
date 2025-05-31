/*
 * adc_input_map.h
 * Schematic‑verified analog map for the D20 v1.1 drum module.
 */

#ifndef ADC_INPUT_MAP_H
#define ADC_INPUT_MAP_H

#include <driver/adc.h>
#include <Arduino.h>

// LED Configuration
#define LED_PIN 37   // Pin connected to the LED
#define NUM_LEDS 1   // Number of LEDs in the strip
#define DELAY_TIME 5 // Delay time in milliseconds
#define hihat_delay 6000 //testing delay

// New pin definitions
// Mode select pin
#define HIHAT_SWITCH_1 1
#define HIHAT_SWITCH_2 17 


// MUX 1
#define MUX1_S0 3
#define MUX1_S1 4
#define MUX1_S2 6
#define MUX1_S3 5

// MUX2
#define MUX2_S0 8
#define MUX2_S1 9
#define MUX2_S2 11
#define MUX2_S3 10

// ADC read pins
#define MUX1_ADC 2
#define MUX2_ADC 7

// Hihat channels
#define HH1channel 5 //mux1
#define HH2channel 18 //direct

// Map MUX readings to correct ADC channels based on your mapping
int mux1_mapping[16] = {7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8};
int mux2_mapping[16] = {23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24};

//TODO:  add an array with the tip,ring indicator included in the value 
//for the 16 values of mux1 and mux2




//direct channels: 
int direct_adc_mapping[6] = {12, 13, 14, 15, 16, 18};


// labels for each MUX_mapping indexed value
String getInputLabel(int adcChannel) {
    switch(adcChannel) {
      case 0: return "Input 1, Ring, ADC0";  
      case 1: return "Input 1, Tip, ADC1";
      case 2: return "Input 2, Mono, ADC2"; 
      case 3: return "Input 4, Ring, ADC3";
      case 4: return "Input 3, Ring, ADC4";
      case 5: return "Input 4, Tip, ADC5";
      case 6: return "Input 3, Tip, ADC6";
      case 7: return "Input 6, Ring, ADC7";
      case 8: return "Input 5, Ring, ADC8";
      case 9: return "Input 6, Tip, ADC9";
      case 10: return "Input 5, Tip, ADC10";
      case 11: return "Input 8, Ring, ADC11";
      case 12: return "Input 7, Ring, ADC12";
      case 13: return "Input 8, Tip, ADC13";
      case 14: return "Input 7, Tip, ADC14";
      case 15: return "Input 10, Ring, ADC15";
      case 16: return "Input 9, Ring, ADC16";
      case 17: return "Input 10, Tip, ADC17";
      case 18: return "Input 9, Tip, ADC18";
      case 19: return "Input 12, Ring, ADC19";
      case 20: return "Input 12, Tip, ADC20";
      case 21: return "Input 11, Ring, ADC21";
      case 22: return "Input 11, Tip, ADC22";
      case 23: return "Input 14, Ring, ADC23";
      case 24: return "Input 13, Ring, ADC24";
      case 25: return "Input 14, Tip, ADC25";
      case 26: return "Input 13, Tip, ADC26";
      case 27: return "Input 16, Ring, ADC27";
      case 28: return "Input 15, Ring, ADC28";
      case 29: return "Input 16, Tip, ADC29";
      case 30: return "Input 15, Tip, ADC30";
      case 31: return "Input 18, Ring, ADC31";
      case 32: return "Input 17, Ring, ADC32";
      case 33: return "Input 18, Tip, ADC33";
      case 34: return "Input 17, Tip, ADC34";
      case 35: return "Input 19, Ring, ADC35";
      case 36: return "Input 19, Tip, ADC36";
      case 37: return "Input 20, Mono, ADC37";
      default: return "Unknown ADC" + String(adcChannel);
    }
  }

/**
 * Input Mapping
 * Input 1 Tip = ADC1       
 * Input 1 Ring = ADC0
 * Input 2 Tip = ADC2  //hihat pdl1
 * Input 3 Tip = ADC6
 * Input 3 Ring = ADC4
 * Input 4 Tip = ADC5
 * Input 4 Ring = ADC3
 * Input 5 Tip = ADC10
 * Input 5 Ring = ADC8
 * Input 6 Tip = ADC9
 * Input 6 Ring = ADC7
 * Input 7 Tip = ADC14
 * Input 7 Ring = ADC12
 * Input 8 Tip = ADC13
 * Input 8 Ring = ADC11
 * Input 9 Tip = ADC18
 * Input 9 Ring = ADC16
 * Input 10 Tip = ADC17
 * Input 10 Ring = ADC15
 * Input 11 Tip = ADC22
 * Input 11 Ring = ADC21
 * Input 12 Tip = ADC20
 * Input 12 Ring = ADC19
 * Input 13 Tip = ADC26
 * Input 13 Ring = ADC24
 * Input 14 Tip = ADC25
 * Input 14 Ring = ADC23
 * Input 15 Tip = ADC30
 * Input 15 Ring = ADC28
 * Input 16 Tip = ADC29
 * Input 16 Ring = ADC27
 * Input 17 Tip = ADC34
 * Input 17 Ring = ADC32
 * Input 18 Tip = ADC33
 * Input 18 Ring = ADC31
 * Input 19 Tip = ADC36
 * Input 19 Ring = ADC35
 * Input 20 Tip = ADC37  //hihat pdl2
 */

#endif  // ADC_INPUT_MAP_H