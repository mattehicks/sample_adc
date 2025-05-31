#include <FastLED.h>
#include <Arduino.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include "adc_input_map.h"
#include "input_config.h"

// MIDI Serial pins
#define RXD2 16
#define TXD2 17

// LED Configuration
#define LED_PIN 37        // Pin connected to the LED
#define NUM_LEDS 1        // Number of LEDs in the strip
#define DELAY_TIME 5      // Delay time in milliseconds

// New pin definitions
#define OUTPUT_PIN 3      // Pin to set HIGH
#define ANALOG_PIN 2      // Pin to read analog value from

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

// Development board variables
#define RXD2 2 // Dev board has no MIDI in but is used for the .begin serial command
#define TXD2 26 // Dev board uses Arduino ESP32 Nano Pin D26 as MIDI Out

int mux1Value = 0;
int mux2Value = 0;

// Arrays to store ADC readings for all channels
int mux1Values[16] = {0};
int mux2Values[16] = {0};
unsigned long lastADCPrintTime = 0;

// LED array
CRGB leds[NUM_LEDS];

// Variable to store analog reading
int analogValue = 0;

void setupMuxPins() {
  // Set all MUX control pins as outputs
  pinMode(MUX1_S0, OUTPUT);
  pinMode(MUX1_S1, OUTPUT);
  pinMode(MUX1_S2, OUTPUT);
  pinMode(MUX1_S3, OUTPUT);
  
  pinMode(MUX2_S0, OUTPUT);
  pinMode(MUX2_S1, OUTPUT);
  pinMode(MUX2_S2, OUTPUT);
  pinMode(MUX2_S3, OUTPUT);
  
  // Set ADC pins as inputs
  pinMode(MUX1_ADC, INPUT);
  pinMode(MUX2_ADC, INPUT);
}

// Function to set MUX channel (0-15)
void setMuxChannel(byte channel, byte s0, byte s1, byte s2, byte s3) {
  digitalWrite(s0, (channel & 1) ? HIGH : LOW);        // bit 0
  digitalWrite(s1, (channel & 2) ? HIGH : LOW);        // bit 1
  digitalWrite(s2, (channel & 4) ? HIGH : LOW);        // bit 2
  digitalWrite(s3, (channel & 8) ? HIGH : LOW);        // bit 3
}

void sendMIDI(uint8_t channel, uint8_t note, uint8_t velocity) {
    // MIDI Note On message: 0x90 + channel, note, velocity
    Serial2.write(0x90 | (channel - 1));
    Serial2.write(note);
    Serial2.write(velocity);
}

void scanADC() {   
    static byte currentChannel = 0;   
    static unsigned long scanStartTime = 0;      
    
    // Arrays to store all ADC values
    static int adcValues[38]; // ADC0-ADC37
    
    // Scan all MUX channels quickly   
    if (millis() - scanStartTime >= 5) {  // Small delay between channel changes     
        scanStartTime = millis();          
        
   // Set channel on both MUXes     
      setMuxChannel(currentChannel, MUX1_S0, MUX1_S1, MUX1_S2, MUX1_S3);     
      setMuxChannel(currentChannel, MUX2_S0, MUX2_S1, MUX2_S2, MUX2_S3);          
      
      // Read ADC values after a tiny delay to allow MUX to settle     
      delayMicroseconds(50);     
      mux1Value = analogRead(MUX1_ADC);     
      mux2Value = analogRead(MUX2_ADC);
      
        // Handle MUX1 triggered inputs
        if (mux1Value > 10)  { 
          //get midi properties from the (40 value) input array
            const InputConfig& config = inputConfigs[mux1_mapping[currentChannel]];
            uint8_t velocity = map(mux1Value, 0, 4095, 0, 127);
            sendMIDI(config.channel, config.notePri, velocity);

            //tip,ring labels from 16 values for mux1
            String inputLabel = getInputLabel(mux1_mapping[currentChannel]);
            Serial.println(inputLabel + " vel: " + String(velocity)); 
        }

        // Handle MUX2 triggered inputs
        if (mux2Value > 10)  {
          //get midi properties from the (40 value) input array
            const InputConfig& config = inputConfigs[mux2_mapping[currentChannel] + 16];
            uint8_t velocity = map(mux2Value, 0, 4095, 0, 127);
            sendMIDI(config.channel, config.notePri, velocity);

            //tip,ring labels from 16 values for mux2
            String inputLabel = getInputLabel(mux2_mapping[currentChannel]);
            Serial.println(inputLabel + " vel: " + String(velocity));
        }
        
        // Move to next channel     
        currentChannel = (currentChannel + 1) % 16;   
    }      
    
    // Read direct ADC pins (ADC32-37) every scan cycle
    adcValues[32] = analogRead(12); // ADC32 = pin 12
    adcValues[33] = analogRead(13); // ADC33 = pin 13
    adcValues[34] = analogRead(14); // ADC34 = pin 14
    adcValues[35] = analogRead(15); // ADC35 = pin 15
    adcValues[36] = analogRead(16); // ADC36 = pin 16
    adcValues[37] = analogRead(18); // ADC37 = pin 18

//todo : print direct adc values somehow


}

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);  // Use Serial for debug output
    
    // Initialize MIDI serial communication on serial2
    Serial2.begin(31250, SERIAL_8N1, RXD2, TXD2);

    setupMuxPins(); // Setup MUX pins
    
    // Initialize FastLED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

    FastLED.setBrightness(0); // Start with brightness at 0
    
    // Configure ADC resolution and attenuation
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set attenuation to 11dB for full 0-3.3V range
    
    // Configure ADC pins
    pinMode(MUX1_ADC, INPUT);
    pinMode(MUX2_ADC, INPUT);
    
    // Configure direct ADC pins
    pinMode(12, INPUT);  // ADC32
    pinMode(13, INPUT);  // ADC33
    pinMode(14, INPUT);  // ADC34
    pinMode(15, INPUT);  // ADC35
    pinMode(16, INPUT);  // ADC36
    pinMode(18, INPUT);  // ADC37

    Serial.println("Setup complete, pin 3 set HIGH");
}

void loop() {
    static uint8_t brightness = 0;  // Current brightness level
    static int8_t direction = 1;    // Direction of brightness change (+1 or -1)
    static unsigned long lastUpdate = 0; // Last time brightness was updated

    // Check if it's time to update the brightness
    if (millis() - lastUpdate >= DELAY_TIME) {
        lastUpdate = millis(); // Update the time

        // Set the LED color to dark red with the current brightness
        FastLED.setBrightness(brightness);
        fill_solid(leds, NUM_LEDS, CRGB::DarkRed); // Set all LEDs to dark red
        FastLED.show();

        // Update brightness
        brightness += direction;

        // Reverse direction at max or min brightness
        if (brightness == 0 || brightness == 255) {
            direction = -direction;
        }
    }

    scanADC(); // Call the function to scan ADC values
}

