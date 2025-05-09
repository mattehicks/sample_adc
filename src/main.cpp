#include <Arduino.h>
#include <driver/adc.h>
#include <FastLED.h>
#include "adc_input_map.h"

#define NUM_LEDS 3
#define LED_COMM_PIN 35
#define LED_STATUS_PIN 36
#define LED_POWER_PIN 37

CRGB leds[NUM_LEDS];

static const uint32_t STEP_DELAY_MS = 250;   // 250 ms between reads

// Select a channel on CD74HC4067 (mux = 1 or 2).
static void selectMuxChannel(uint8_t mux, uint8_t ch)
{
    uint8_t s0 = (ch & 0x01);
    uint8_t s1 = (ch & 0x02) >> 1;
    uint8_t s2 = (ch & 0x04) >> 2;
    uint8_t s3 = (ch & 0x08) >> 3;

    if (mux == 1) {
        digitalWrite(MUX1_S0_GPIO, s0);
        digitalWrite(MUX1_S1_GPIO, s1);
        digitalWrite(MUX1_S2_GPIO, s2);
        digitalWrite(MUX1_S3_GPIO, s3);
    } else {
        digitalWrite(MUX2_S0_GPIO, s0);
        digitalWrite(MUX2_S1_GPIO, s1);
        digitalWrite(MUX2_S2_GPIO, s2);
        digitalWrite(MUX2_S3_GPIO, s3);
    }
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
    pinMode(MUX1_S0_GPIO, OUTPUT);
    pinMode(MUX1_S1_GPIO, OUTPUT);
    pinMode(MUX1_S2_GPIO, OUTPUT);
    pinMode(MUX1_S3_GPIO, OUTPUT);

    pinMode(MUX2_S0_GPIO, OUTPUT);
    pinMode(MUX2_S1_GPIO, OUTPUT);
    pinMode(MUX2_S2_GPIO, OUTPUT);
    pinMode(MUX2_S3_GPIO, OUTPUT);

    // Configure direct ADC GPIOs as inputs (no pull-downs for raw testing)
    pinMode(14, INPUT);  // Jack17 A
    pinMode(12, INPUT);  // Jack17 B
    pinMode(13, INPUT);  // Jack18 A
    pinMode(15, INPUT);  // Jack19 B
    pinMode(16, INPUT);  // Jack19 A
    pinMode(18, INPUT);  // Jack20 single

    // Configure ADC resolution and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);
    // ADC2 width is configured per-read, no need for global config

    // Configure ADC1 attenuation - using 0dB for raw testing
    adc1_config_channel_atten(MUX1_OUT_ADC, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MUX2_OUT_ADC, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_0);

    // Configure ADC2 attenuation - using 0dB for raw testing
    adc2_config_channel_atten(ADC2_CHANNEL_1, ADC_ATTEN_DB_0);
    adc2_config_channel_atten(ADC2_CHANNEL_2, ADC_ATTEN_DB_0);
    adc2_config_channel_atten(ADC2_CHANNEL_3, ADC_ATTEN_DB_0);
    adc2_config_channel_atten(ADC2_CHANNEL_4, ADC_ATTEN_DB_0);
    adc2_config_channel_atten(ADC2_CHANNEL_5, ADC_ATTEN_DB_0);
}

static inline void settleWithGround()
{
    //trying to settle the ADC with a known 0V input
    // 1. Select MUX1 CH0 (known to be idle and near 0 V)
    selectMuxChannel(1, 0);
    delayMicroseconds(3);
    adc1_get_raw(MUX1_OUT_ADC);   // sample ~0 V, charge S/H at ground
}

// Forces the ADC's sample‑and‑hold capacitor to 0 V, then returns a clean reading.
static int stableRead(const DirectADC& input)
{
    // Drive the S/H cap to ground by sampling a known‑low source:
    //   MUX1 channel 0 is connected to Input 6 B and is near 0 V when idle.
    selectMuxChannel(1, 0);
    delayMicroseconds(3);
    adc1_get_raw(MUX1_OUT_ADC);   // ground sample

    // Optional 30‑µs pause to let internal pulldown bleed any residual
    ets_delay_us(30);

    // Read from appropriate ADC
    if (input.adc.adc1 != ADC1_CHANNEL_MAX) {
        // ADC1 reading
        adc1_get_raw(input.adc.adc1);  // dummy read
        return adc1_get_raw(input.adc.adc1);
    } else {
        // ADC2 reading
        int raw;
        adc2_get_raw(input.adc.adc2, ADC_WIDTH_BIT_12, &raw);  // dummy read
        adc2_get_raw(input.adc.adc2, ADC_WIDTH_BIT_12, &raw);
        return raw;
    }
}

void loop()
{
    // ---------- Scan MUX 1 ----------
    for (uint8_t ch = 0; ch < 16; ++ch) {
        selectMuxChannel(1, ch);
        delayMicroseconds(1);                     // reduced settle time for raw testing
        int val = adc1_get_raw(MUX1_OUT_ADC);
        uint8_t inputNum = mux1InputMap[ch];
        // Output: <inputNum> <MUX1_CHx> <value> (tab-separated)
        Serial.printf("%u\tMUX1_CH%u\t%d\n", inputNum, ch, val);
        delay(STEP_DELAY_MS);
    }

    // ---------- Scan MUX 2 ----------
    for (uint8_t ch = 0; ch < 16; ++ch) {
        selectMuxChannel(2, ch);
        delayMicroseconds(1);                     // reduced settle time for raw testing
        int val = adc1_get_raw(MUX2_OUT_ADC);
        uint8_t inputNum = mux2InputMap[ch];
        Serial.printf("%u\tMUX2_CH%u\t%d\n", inputNum, ch, val);
        delay(STEP_DELAY_MS);
    }

    // ---------- Scan direct ADC inputs ----------
    // adjusted for floating inputs corrections  
    for (uint8_t idx = 0; idx < 4; ++idx) {   // inputs 17‑20
        uint8_t inputNum = 17 + idx;

        for (uint8_t cond = 0; cond < 2; ++cond) {
            uint8_t gpio = directInputs[idx][cond].gpio;
            if (gpio == 255) continue;        // unused

            const DirectADC& input = directInputs[idx][cond];

            // Jack 18 second conductor (muxed through MUX2 CH8)
            if (inputNum == 18 && cond == 1) {
                selectMuxChannel(2, 8);
                delayMicroseconds(3);
            }

            int val = stableRead(input);      // << use the helper
            Serial.printf("%u\tGPIO%u\t%d\n", inputNum, gpio, val);
            delay(STEP_DELAY_MS);
        }
    }

    // // ---------- Scan direct ADC inputs ----------
    // for (uint8_t idx = 0; idx < 4; ++idx) {       // inputs 17‑20
    //     uint8_t inputNum = 17 + idx;
    //     for (uint8_t cond = 0; cond < 2; ++cond) {
    //         uint8_t gpio = directInputs[idx][cond].gpio;
    //         if (gpio == 255) continue;            // unused
    //         adc1_channel_t adcCh = directInputs[idx][cond].adc;
    //         adc1_config_channel_atten(adcCh, ADC_ATTEN_DB_11);
    //         int val = adc1_get_raw(adcCh);
    //         Serial.printf("%u\tGPIO%u\t%d\n", inputNum, gpio, val);
    //         delay(STEP_DELAY_MS);
    //     }
    // }
}

