#ifndef SNES_PINS_H
#define SNES_PINS_H

// =============================================================================
// SNES Video Input Pins (Current Test Mapping)
// =============================================================================
// Pins 20, 21, 22 as used in current breadboard/setup

#define PIN_SNES_PCLK  20
#define PIN_SNES_R0    21
#define PIN_SNES_CSYNC 22

// Frequency Counter Pins (aliased for now)
#define PIN_FREQ_PCLK  PIN_SNES_PCLK
#define PIN_FREQ_R0    PIN_SNES_R0
#define PIN_FREQ_CSYNC PIN_SNES_CSYNC

#endif // SNES_PINS_H
