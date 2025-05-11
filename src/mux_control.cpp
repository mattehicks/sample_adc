#include "mux_control.h"

void selectMuxChannel(uint8_t mux, uint8_t ch)
{
    // Validate inputs
    if (ch > 15) {
        Serial.printf("Invalid channel %d for MUX %d\n", ch, mux);
        return;  // Invalid channel
    }
    if (mux != 1 && mux != 2) {
        Serial.printf("Invalid MUX number %d\n", mux);
        return;  // Invalid mux
    }

    // Extract select bits (s0=LSB, s3=MSB)
    uint8_t s0 = (ch & 0x01);        // Bit 0
    uint8_t s1 = (ch & 0x02) >> 1;   // Bit 1
    uint8_t s2 = (ch & 0x04) >> 2;   // Bit 2
    uint8_t s3 = (ch & 0x08) >> 3;   // Bit 3

    if (mux == 1) {
        // First, read current state
        uint8_t pre_s0 = digitalRead(MUX1_S0_GPIO);
        uint8_t pre_s1 = digitalRead(MUX1_S1_GPIO);
        uint8_t pre_s2 = digitalRead(MUX1_S2_GPIO);
        uint8_t pre_s3 = digitalRead(MUX1_S3_GPIO);
        
        Serial.printf("MUX1: Before setting CH%d - Current state=%d%d%d%d\n",
                     ch, pre_s3, pre_s2, pre_s1, pre_s0);

        // Refresh pin modes to ensure they're in output mode
        pinMode(MUX1_S0_GPIO, OUTPUT);
        pinMode(MUX1_S1_GPIO, OUTPUT);
        pinMode(MUX1_S2_GPIO, OUTPUT);
        pinMode(MUX1_S3_GPIO, OUTPUT);
        
        // Set pins individually with small delays
        digitalWrite(MUX1_S3_GPIO, s3);
        delayMicroseconds(5);
        digitalWrite(MUX1_S2_GPIO, s2);
        delayMicroseconds(5);
        digitalWrite(MUX1_S1_GPIO, s1);
        delayMicroseconds(5);
        digitalWrite(MUX1_S0_GPIO, s0);
        
        // Give the MUX more time to settle
        delayMicroseconds(50);
        
        // Read back and verify
        uint8_t read_s0 = digitalRead(MUX1_S0_GPIO);
        uint8_t read_s1 = digitalRead(MUX1_S1_GPIO);
        uint8_t read_s2 = digitalRead(MUX1_S2_GPIO);
        uint8_t read_s3 = digitalRead(MUX1_S3_GPIO);
        
        Serial.printf("MUX1: CH%d, Want S[3:0]=%d%d%d%d, Got=%d%d%d%d\n", 
                     ch, s3, s2, s1, s0, read_s3, read_s2, read_s1, read_s0);
                     
        if (read_s0 != s0 || read_s1 != s1 || read_s2 != s2 || read_s3 != s3) {
            Serial.println("Warning: MUX1 channel selection verification failed");
            Serial.printf("  Failed bits: %s%s%s%s\n",
                         (read_s3 != s3) ? "S3 " : "",
                         (read_s2 != s2) ? "S2 " : "",
                         (read_s1 != s1) ? "S1 " : "",
                         (read_s0 != s0) ? "S0 " : "");
            
            // Try setting the pins again with longer delay and individual writes
            pinMode(MUX1_S3_GPIO, OUTPUT);
            digitalWrite(MUX1_S3_GPIO, s3);
            delayMicroseconds(10);
            
            pinMode(MUX1_S2_GPIO, OUTPUT);
            digitalWrite(MUX1_S2_GPIO, s2);
            delayMicroseconds(10);
            
            pinMode(MUX1_S1_GPIO, OUTPUT);
            digitalWrite(MUX1_S1_GPIO, s1);
            delayMicroseconds(10);
            
            pinMode(MUX1_S0_GPIO, OUTPUT);
            digitalWrite(MUX1_S0_GPIO, s0);
            delayMicroseconds(100);  // Even longer final delay
        }
    } else if (mux == 2) {  // Properly enclosed MUX2 section
        // First, read current state for MUX2
        uint8_t pre_s0 = digitalRead(MUX2_S0_GPIO);
        uint8_t pre_s1 = digitalRead(MUX2_S1_GPIO);
        uint8_t pre_s2 = digitalRead(MUX2_S2_GPIO);
        uint8_t pre_s3 = digitalRead(MUX2_S3_GPIO);
        
        Serial.printf("MUX2: Before setting CH%d - Current state=%d%d%d%d\n",
                     ch, pre_s3, pre_s2, pre_s1, pre_s0);
        
        // Set all pins at once to avoid glitches
        digitalWrite(MUX2_S0_GPIO, s0);
        digitalWrite(MUX2_S1_GPIO, s1);
        digitalWrite(MUX2_S2_GPIO, s2);
        digitalWrite(MUX2_S3_GPIO, s3);
        
        // Give the MUX time to settle
        delayMicroseconds(50);
        
        // Read back and verify
        uint8_t read_s0 = digitalRead(MUX2_S0_GPIO);
        uint8_t read_s1 = digitalRead(MUX2_S1_GPIO);
        uint8_t read_s2 = digitalRead(MUX2_S2_GPIO);
        uint8_t read_s3 = digitalRead(MUX2_S3_GPIO);
        
        Serial.printf("MUX2: CH%d, S[3:0]=%d%d%d%d, Read=%d%d%d%d\n", 
                     ch, s3, s2, s1, s0, read_s3, read_s2, read_s1, read_s0);
                     
        if (read_s0 != s0 || read_s1 != s1 || read_s2 != s2 || read_s3 != s3) {
            Serial.println("Warning: MUX2 channel selection verification failed");
            Serial.printf("  Failed bits: %s%s%s%s\n",
                         (read_s3 != s3) ? "S3 " : "",
                         (read_s2 != s2) ? "S2 " : "",
                         (read_s1 != s1) ? "S1 " : "",
                         (read_s0 != s0) ? "S0 " : "");
            
            // Try setting the pins again with longer delay
            digitalWrite(MUX2_S0_GPIO, s0);
            digitalWrite(MUX2_S1_GPIO, s1);
            digitalWrite(MUX2_S2_GPIO, s2);
            digitalWrite(MUX2_S3_GPIO, s3);
            delayMicroseconds(100);
        }
    }
} 