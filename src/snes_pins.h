#ifndef SNES_PINS_H
#define SNES_PINS_H

// =============================================================================
// SNES Video Input Pins
// =============================================================================

#define PIN_SNES_PCLK   20
#define PIN_SNES_R0     21
#define PIN_SNES_CSYNC  22 // Optional now, but kept for reference

// Hard Sync Pins (Added for perfect stability)
#define PIN_SNES_HBLANK 26 // Connect to PPU2 Pin 25
#define PIN_SNES_VBLANK 27 // Connect to PPU2 Pin 26

// Frequency Counter Pins (aliased for debug tools)
#define PIN_FREQ_PCLK  PIN_SNES_PCLK
#define PIN_FREQ_R0    PIN_SNES_R0
#define PIN_FREQ_CSYNC PIN_SNES_CSYNC

#endif // SNES_PINS_H
