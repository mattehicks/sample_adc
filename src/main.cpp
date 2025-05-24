#include <Arduino.h>
#include <driver/adc.h>
#include <FastLED.h>
#include "adc_input_map.h"

#define NUM_LEDS 3
#define LED_COMM_PIN 35
#define LED_STATUS_PIN 36
#define LED_POWER_PIN 37
  

CRGB leds[NUM_LEDS];
  
//True for high-hat mode or
//False for trigger mode.
bool HH1_mode = false;
bool HH2_mode = false;

// Function to set MUX channel (0-15)
void setMuxChannel(byte channel, byte s0, byte s1, byte s2, byte s3) {
  digitalWrite(s0, (channel & 1) ? HIGH : LOW);        // bit 0
  digitalWrite(s1, (channel & 2) ? HIGH : LOW);        // bit 1
  digitalWrite(s2, (channel & 4) ? HIGH : LOW);        // bit 2
  digitalWrite(s3, (channel & 8) ? HIGH : LOW);        // bit 3
}

void setup()    
{
    Serial.begin(115200);
    // Setup LEDs
    FastLED.addLeds<WS2812, LED_COMM_PIN, GRB>(leds, 0, 1);
    FastLED.addLeds<WS2812, LED_STATUS_PIN, GRB>(leds, 1, 1);
    FastLED.addLeds<WS2812, LED_POWER_PIN, GRB>(leds, 2, 1);
    
    // Set all LEDs to green
    leds[0] = CRGB::Green;  // COMM LED
    leds[1] = CRGB::Green;  // STATUS LED
    leds[2] = CRGB::Green;  // POWER LED
    FastLED.show();

    // Configure select pins
    pinMode(MUX1_S0, OUTPUT);
    pinMode(MUX1_S1, OUTPUT);
    pinMode(MUX1_S2, OUTPUT);
    pinMode(MUX1_S3, OUTPUT);

    pinMode(MUX2_S0, OUTPUT);
    pinMode(MUX2_S1, OUTPUT);
    pinMode(MUX2_S2, OUTPUT);
    pinMode(MUX2_S3, OUTPUT);

    //Hihat mode is trigger by outputting a low for high-hat mode or a HIGH for trigger mode.
    pinMode(HIHAT_SWITCH_1, OUTPUT);
    pinMode(HIHAT_SWITCH_2, OUTPUT);

    //Hihat modes:  LOW for high-hat mode or HIGH for trigger mode.
    digitalWrite(HIHAT_SWITCH_1, HH1_mode ? LOW : HIGH);
    digitalWrite(HIHAT_SWITCH_2, HH2_mode ? LOW : HIGH);

    // Configure direct ADC GPIOs as inputs (no pull-downs for raw testing)
    pinMode(14, INPUT);  // Jack17 A
    pinMode(12, INPUT);  // Jack17 B
    pinMode(13, INPUT);  // Jack18 A
    pinMode(15, INPUT);  // Jack19 B
    pinMode(16, INPUT);  // Jack19 A
    pinMode(18, INPUT);  // Jack20 single
    pinMode(MUX1_ADC, INPUT);  // MUX1 ADC input
    pinMode(MUX2_ADC, INPUT);  // MUX2 ADC input

    // Configure ADC resolution and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);
    analogSetAttenuation(ADC_11db);
}
 
 //normalize the hihat input to a value between 0 and 1
float normalizeADC(int pedalValue) { 
  float normalized = (float)(pedalValue - 1500) / (3700 - 1500);
  normalized = fmax(0.0f, fmin(1.0f, normalized));  // Clamp
  return normalized;
}

void readHH1() {
  //set the mux channel to input 2 (MUX1 channel 5)
  setMuxChannel(HH1channel, MUX1_S0, MUX1_S1, MUX1_S2, MUX1_S3);
  // Read the input
  int val = analogRead(MUX1_ADC);
  float normalized = normalizeADC(val);
  Serial.printf("\rHH1: %d (%.2f)    ", val, normalized);
}

void readHH2() {
  //read direct ADC pin for hihat2
  int val = analogRead(HH2channel);
  float normalized = normalizeADC(val);
  Serial.printf("HH2: %d (%.2f)    ", val, normalized);
}
// ADC to Input mapping lookup table
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
 
void scanADC() {   
  // ---------- Scan MUX 1 ----------
  for (uint8_t ch = 0; ch < 16; ++ch) {
    //skip the hihat channels
    if (ch == HH1channel) {
      continue;
    }
      setMuxChannel(ch, MUX1_S0, MUX1_S1, MUX1_S2, MUX1_S3);
      delayMicroseconds(50);
      int val = analogRead(MUX1_ADC);
      uint8_t inputNum = mux1InputMap[ch]; 
      if (val > 20) {
          Serial.printf("%u\tMUX1_CH%u\t%d\n", inputNum, ch, val);
      }
  }

  // ---------- Scan MUX 2 ----------
  for (uint8_t ch = 0; ch < 16; ++ch) {
      setMuxChannel(ch, MUX2_S0, MUX2_S1, MUX2_S2, MUX2_S3);
      delayMicroseconds(50);                    
      int val = analogRead(MUX2_ADC);
      uint8_t inputNum = mux2InputMap[ch];
      if (val > 20) {
          Serial.printf("%u\tMUX2_CH%u\t%d\n", inputNum, ch, val);
      }
  }

  // Read direct ADC pins (ADC32-37) 
  // ---------- Scan direct inputs 17-20 ----------
  for (uint8_t direct = 0; direct < 6; direct++) {
    //skip the hihat channels
    uint8_t inputNum = direct_adc_mapping[direct];
    if (inputNum == HH2channel) {
      continue;
    }
      int val = analogRead(direct_adc_mapping[direct]);

      if (val > 20) {
          Serial.printf("%u\tDIRECT_PIN%u\t%d\n", inputNum, direct, val);
      }
      delayMicroseconds(50); 
  }


}


void loop() {
    scanADC();

    static unsigned long lastToggleTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastToggleTime >= hihat_delay) {  // hihat_delay is 5000ms defined in adc_input_map.h
        // Toggle HH1 mode and switch
        HH1_mode = !HH1_mode;
        digitalWrite(HIHAT_SWITCH_1, HH1_mode);

        // Toggle HH2 mode and switch  
        HH2_mode = !HH2_mode;
        digitalWrite(HIHAT_SWITCH_2, HH2_mode);
        Serial.printf("\n----------------Hihat mode: %d\n", HH1_mode);

        lastToggleTime = currentTime;
    }

  // readHH1();
  // delay(100);
  // readHH2();
  // delay(100);

}

