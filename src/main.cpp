#include <Arduino.h>
#include <driver/adc.h>
#include <FastLED.h>
#include "adc_input_map.h"
#include "mux_test.h"
#include "mux_control.h"

#define NUM_LEDS 3
#define LED_COMM_PIN 35
#define LED_STATUS_PIN 36
#define LED_POWER_PIN 37
#define ADC_THRESHOLD 500

CRGB leds[NUM_LEDS];

static const uint32_t STEP_DELAY_MS = 100;   // 250 ms between reads
static const uint32_t MUX_SETTLE_US = 50;    // Increased settle time for MUX
static const uint32_t GROUND_SETTLE_US = 50; // Increased ground settle time

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ADC test...");
    Serial.println("ADC Configuration:");
    Serial.println("ADC1 Width: 12-bit");
    Serial.println("ADC Attenuation: 11dB (0-3.3V range)");

    // Setup LEDs
    FastLED.addLeds<WS2812, LED_COMM_PIN, GRB>(leds, 0, 1);
    FastLED.addLeds<WS2812, LED_STATUS_PIN, GRB>(leds, 1, 1);
    FastLED.addLeds<WS2812, LED_POWER_PIN, GRB>(leds, 2, 1);
    
    // Set all LEDs to green
    leds[0] = CRGB::Green;  // COMM LED
    leds[1] = CRGB::Green;  // STATUS LED
    leds[2] = CRGB::Green;  // POWER LED
    FastLED.show();

    // Configure select pins as push-pull outputs
    pinMode(MUX1_S0_GPIO, OUTPUT);
    pinMode(MUX1_S1_GPIO, OUTPUT);
    pinMode(MUX1_S2_GPIO, OUTPUT);
    pinMode(MUX1_S3_GPIO, OUTPUT);

    pinMode(MUX2_S0_GPIO, OUTPUT);
    pinMode(MUX2_S1_GPIO, OUTPUT);
    pinMode(MUX2_S2_GPIO, OUTPUT);
    pinMode(MUX2_S3_GPIO, OUTPUT);

    // Configure MUX outputs as analog inputs
    pinMode(MUX1_OUT_GPIO, INPUT);
    pinMode(MUX2_OUT_GPIO, INPUT);

    // Initialize all MUX pins to LOW
    digitalWrite(MUX1_S0_GPIO, LOW);
    digitalWrite(MUX1_S1_GPIO, LOW);
    digitalWrite(MUX1_S2_GPIO, LOW);
    digitalWrite(MUX1_S3_GPIO, LOW);
    digitalWrite(MUX2_S0_GPIO, LOW);
    digitalWrite(MUX2_S1_GPIO, LOW);
    digitalWrite(MUX2_S2_GPIO, LOW);
    digitalWrite(MUX2_S3_GPIO, LOW);

    // Run MUX stability test
    testMuxStability();

    // Configure ADC resolution and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);
    Serial.println("ADC1 width configured to 12-bit");

    // Configure ADC1 attenuation - using 11dB for better range
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_11);  // MUX1 output
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    Serial.println("ADC1 attenuation configured to 11dB");

    // Test initial ADC readings
    Serial.println("\nInitial ADC Readings Test:");
    selectMuxChannel(1, 0);  // Select MUX1 channel 0
    delayMicroseconds(100);   // Increased settle time
    int test_val = analogRead(MUX1_OUT_GPIO);  // Try analogRead instead
    Serial.printf("MUX1 ADC Raw Value: %d\n", test_val);
}

static inline void settleWithGround()
{
    //trying to settle the ADC with a known 0V input
    // 1. Select MUX1 CH0 (known to be idle and near 0 V)
    selectMuxChannel(1, 0);
    delayMicroseconds(GROUND_SETTLE_US);
    adc1_get_raw(MUX1_OUT_ADC);   // sample ~0 V, charge S/H at ground
}

// Forces the ADC's sample‑and‑hold capacitor to 0 V, then returns a clean reading.
static int stableRead(uint8_t gpio)
{
    // Drive the S/H cap to ground by sampling a known‑low source:
    selectMuxChannel(1, 0);
    delayMicroseconds(GROUND_SETTLE_US);
    adc1_get_raw(MUX1_OUT_ADC);   // ground sample

    // Optional 30‑µs pause to let internal pulldown bleed any residual
    ets_delay_us(30);

    // Read from appropriate ADC based on GPIO
    int raw;
    esp_err_t result;
    switch(gpio) {
        case 12: result = adc2_get_raw(GPIO_12_ADC, ADC_WIDTH_BIT_12, &raw); break;
        case 13: result = adc2_get_raw(GPIO_13_ADC, ADC_WIDTH_BIT_12, &raw); break;
        case 14: result = adc2_get_raw(GPIO_14_ADC, ADC_WIDTH_BIT_12, &raw); break;
        case 15: result = adc2_get_raw(GPIO_15_ADC, ADC_WIDTH_BIT_12, &raw); break;
        case 16: result = adc2_get_raw(GPIO_16_ADC, ADC_WIDTH_BIT_12, &raw); break;
        case 18: result = adc2_get_raw(GPIO_18_ADC, ADC_WIDTH_BIT_12, &raw); break;
        default: return 0;  // Invalid GPIO
    }
    
    if (result != ESP_OK) {
        Serial.printf("ADC read error on GPIO %d: %d\n", gpio, result);
        return 0;
    }
    return raw;
}

// Function to check ADC value and update STATUS LED
static void checkADCAndUpdateLED(int adcValue) {
    if (adcValue > ADC_THRESHOLD) {
        leds[1] = CRGB::Red;  // STATUS LED to red
    } else {
        leds[1] = CRGB::Green;  // STATUS LED back to green
    }
    FastLED.show();
}

// Read all direct ADC inputs (inputs 17-20)
static void readDirectInputs() {
    // Input 17 (TIP and RING)
    int val17_tip = stableRead(14);  // GPIO 14
    int val17_ring = stableRead(12); // GPIO 12
    Serial.printf("17\tGPIO14\t\t%d\n", val17_tip);
    Serial.printf("17\tGPIO12\t\t%d\n", val17_ring);
    checkADCAndUpdateLED(val17_tip);
    checkADCAndUpdateLED(val17_ring);
    delay(STEP_DELAY_MS);

    // Input 18 (TIP only)
    int val18 = stableRead(13);  // GPIO 13
    Serial.printf("18\tGPIO13\t\t%d\n", val18);
    checkADCAndUpdateLED(val18);
    delay(STEP_DELAY_MS);

    // Input 19 (TIP and RING)
    int val19_tip = stableRead(16);  // GPIO 16
    int val19_ring = stableRead(15); // GPIO 15
    Serial.printf("19\tGPIO16\t\t%d\n", val19_tip);
    Serial.printf("19\tGPIO15\t\t%d\n", val19_ring);
    checkADCAndUpdateLED(val19_tip);
    checkADCAndUpdateLED(val19_ring);
    delay(STEP_DELAY_MS);

    // Input 20 (single)
    int val20 = stableRead(18);  // GPIO 18
    Serial.printf("20\tGPIO18\t\t%d\n", val20);
    checkADCAndUpdateLED(val20);
    delay(STEP_DELAY_MS);
}

void loop()
{
    // First test direct ADC reading without MUX
    Serial.println("\n=== Testing Direct ADC Reading ===");
    int direct_val = analogRead(MUX1_OUT_GPIO);  // Try analogRead
    Serial.printf("Direct MUX1_OUT_GPIO reading: %d (0x%03X)\n", direct_val, direct_val);
    
    // Test all MUX1 channels
    Serial.println("\n=== Testing All MUX1 Channels ===");
    
    for(uint8_t ch = 0; ch < 16; ch++) {
        Serial.printf("\nTesting MUX1 Channel %d (Input %d):\n", ch, mux1InputMap[ch]);
        
        // Set channel
        selectMuxChannel(1, ch);
        delayMicroseconds(100);  // Increased settle time
        
        // Take multiple readings to check for any activity
        for(int i = 0; i < 3; i++) {
            int val = analogRead(MUX1_OUT_GPIO);  // Try analogRead
            Serial.printf("  Reading %d: Raw=%d (0x%03X)\n", i, val, val);
            if (val > 0) {
                //short circuit - stop if value found.
                leds[1] = CRGB::Red;  // STATUS LED to red
                FastLED.show();
                while (1) { delay(1000); } // Pause loop indefinitely
            }
            delay(10);
        }
    }
    
    Serial.println("\n=== End of MUX1 Test Cycle ===");
    delay(1000); // Wait 1 second between test cycles
}

