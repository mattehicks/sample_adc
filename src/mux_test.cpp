#include "mux_test.h"
#include "mux_control.h"

// Test function to verify MUX state stability
void testMuxStability(void) {
    Serial.println("\n=== MUX Stability Test ===");
    
    // Test MUX1
    Serial.println("\nTesting MUX1:");
    for (uint8_t ch = 0; ch < 16; ++ch) {
        Serial.printf("\nTesting MUX1 Channel %d:\n", ch);
        
        // Set channel
        selectMuxChannel(1, ch);
        
        // Verify state multiple times
        for (int i = 0; i < 5; i++) {
            delay(10); // Wait between reads
            
            // Read back state
            uint8_t read_s0 = digitalRead(MUX1_S0_GPIO);
            uint8_t read_s1 = digitalRead(MUX1_S1_GPIO);
            uint8_t read_s2 = digitalRead(MUX1_S2_GPIO);
            uint8_t read_s3 = digitalRead(MUX1_S3_GPIO);
            
            // Calculate expected values
            uint8_t exp_s0 = (ch & 0x01);
            uint8_t exp_s1 = (ch & 0x02) >> 1;
            uint8_t exp_s2 = (ch & 0x04) >> 2;
            uint8_t exp_s3 = (ch & 0x08) >> 3;
            
            Serial.printf("Read %d: S[3:0]=%d%d%d%d, Expected=%d%d%d%d\n",
                         i, read_s3, read_s2, read_s1, read_s0,
                         exp_s3, exp_s2, exp_s1, exp_s0);
                         
            if (read_s0 != exp_s0 || read_s1 != exp_s1 || 
                read_s2 != exp_s2 || read_s3 != exp_s3) {
                Serial.println("WARNING: MUX1 state mismatch!");
            }
        }
    }
    
    // Test MUX2
    Serial.println("\nTesting MUX2:");
    for (uint8_t ch = 0; ch < 16; ++ch) {
        Serial.printf("\nTesting MUX2 Channel %d:\n", ch);
        
        // Set channel
        selectMuxChannel(2, ch);
        
        // Verify state multiple times
        for (int i = 0; i < 5; i++) {
            delay(10); // Wait between reads
            
            // Read back state
            uint8_t read_s0 = digitalRead(MUX2_S0_GPIO);
            uint8_t read_s1 = digitalRead(MUX2_S1_GPIO);
            uint8_t read_s2 = digitalRead(MUX2_S2_GPIO);
            uint8_t read_s3 = digitalRead(MUX2_S3_GPIO);
            
            // Calculate expected values
            uint8_t exp_s0 = (ch & 0x01);
            uint8_t exp_s1 = (ch & 0x02) >> 1;
            uint8_t exp_s2 = (ch & 0x04) >> 2;
            uint8_t exp_s3 = (ch & 0x08) >> 3;
            
            Serial.printf("Read %d: S[3:0]=%d%d%d%d, Expected=%d%d%d%d\n",
                         i, read_s3, read_s2, read_s1, read_s0,
                         exp_s3, exp_s2, exp_s1, exp_s0);
                         
            if (read_s0 != exp_s0 || read_s1 != exp_s1 || 
                read_s2 != exp_s2 || read_s3 != exp_s3) {
                Serial.println("WARNING: MUX2 state mismatch!");
            }
        }
    }
} 