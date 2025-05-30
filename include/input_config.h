#ifndef INPUT_CONFIG_H
#define INPUT_CONFIG_H

#include <stdint.h>

// Structure to hold configuration for each input
struct InputConfig {
    uint8_t inputNumber;     // 1–20
    const char* name;        // Descriptive name
    uint8_t channel;         // MIDI channel
    uint8_t notePri;         // Primary MIDI note
    uint8_t noteSec;         // Secondary MIDI note
    uint8_t noteRim;         // Rim MIDI note
    uint8_t threshold;       // Trigger threshold
    uint8_t sensitivity;     // Sensitivity value
    uint8_t debounce;        // Debounce setting
    uint8_t duration;        // Minimum duration of trigger
};

// Define configuration for inputs 1–20
const InputConfig inputConfigs[] = {
    { 1,  "HIHat",      1, 42, 0,  0,  5, 10, 0, 0 },
    { 2,  "HIHat_PDL",  1,  1, 0,  0,  5, 10, 0, 0 },
    { 3,  "Snare",      1, 38, 0, 40, 5, 10, 0, 0 },
    { 4,  "Kick",       1, 36, 0,  0,  5, 10, 0, 0 },
    { 5,  "Toms1",      1, 48, 0, 50, 5, 10, 0, 0 },
    { 6,  "Toms2",      1, 45, 0, 47, 5, 10, 0, 0 },
    { 7,  "Toms3",      1, 43, 0, 58, 5, 10, 0, 0 },
    { 8,  "Crash1",     1, 55, 0, 49, 5, 10, 0, 0 },
    { 9,  "Crash2",     1, 52, 0, 57, 5, 10, 0, 0 },
    {10,  "Ride",       1, 51, 0, 57, 5, 10, 0, 0 },
    {11,  "Pad11",      1, 60, 0,  0,  5, 10, 0, 0 },
    {12,  "Pad12",      1, 61, 0,  0,  5, 10, 0, 0 },
    {13,  "Pad13",      1, 62, 0,  0,  5, 10, 0, 0 },
    {14,  "Pad14",      1, 63, 0,  0,  5, 10, 0, 0 },
    {15,  "Pad15",      1, 64, 0,  0,  5, 10, 0, 0 },
    {16,  "Pad16",      1, 65, 0,  0,  5, 10, 0, 0 },
    {17,  "Pad17",      1, 66, 0,  0,  5, 10, 0, 0 },
    {18,  "Pad18",      1, 67, 0,  0,  5, 10, 0, 0 },
    {19,  "Pad19",      1, 68, 0,  0,  5, 10, 0, 0 },
    {20,  "Hihat_PDL2", 1, 1, 0,  0,  5, 10, 0, 0 }
};

const int INPUT_CONFIG_COUNT = sizeof(inputConfigs) / sizeof(InputConfig);

#endif // INPUT_CONFIG_H
