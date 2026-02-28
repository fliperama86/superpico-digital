/**
 * SuperPico Digital - SNES Pin Configuration
 *
 * This file contains all SNES capture and sync pins.
 * Uses the same PCB layout as NeoPico-HD MVS; HBLANK/VBLANK occupy the
 * SHADOW/DARK positions. Contiguous 18-pin capture window: GP27-GP44.
 * CSYNC is not connected on this hardware.
 */

#ifndef SNES_PINS_H
#define SNES_PINS_H

// =============================================================================
// SNES Video Input Pins - GP27-44 CONTIGUOUS LAYOUT
// =============================================================================
// Captures RGB555 + VBLANK + HBLANK from the SNES PPU.
//
// 18-pin capture window: GP27 (LSB) through GP44 (MSB).
//
// Pin mapping (LSB to MSB in captured word):
//   Bit 0:      GP27 (VBLANK) - PPU2 Pin 26
//   Bit 1:      GP28 (PCLK)   - PPU2 Pin 27
//   Bits 2-6:   GP29-33 (Blue B4-B0, contiguous)
//   Bits 7-11:  GP34-38 (Green G4-G0, contiguous)
//   Bits 12-16: GP39-43 (Red R4-R0, contiguous)
//   Bit 17:     GP44 (HBLANK) - PPU2 Pin 25
//
// With PIO GPIOBASE=16, pin index N = GP(N+16). So IN_BASE=11 → GP27.

// Sync / blanking
#define PIN_SNES_VBLANK 27 // Vertical blanking   - Bit 0  (PPU2 Pin 26)
#define PIN_SNES_PCLK   28 // Pixel clock (~5.37 MHz, Bit 1, PPU2 Pin 27)
#define PIN_SNES_BASE   27 // Base pin - capture GP27-44 (18 pins)

// Blue channel (B4-B0) - CONTIGUOUS at bits 2-6
#define PIN_SNES_B4 29
#define PIN_SNES_B3 30
#define PIN_SNES_B2 31
#define PIN_SNES_B1 32
#define PIN_SNES_B0 33

// Green channel (G4-G0) - CONTIGUOUS at bits 7-11
#define PIN_SNES_G4 34
#define PIN_SNES_G3 35
#define PIN_SNES_G2 36
#define PIN_SNES_G1 37
#define PIN_SNES_G0 38

// Red channel (R4-R0) - CONTIGUOUS at bits 12-16
#define PIN_SNES_R4 39
#define PIN_SNES_R3 40
#define PIN_SNES_R2 41
#define PIN_SNES_R1 42
#define PIN_SNES_R0 43

// Blanking
#define PIN_SNES_HBLANK 44 // Horizontal blanking - Bit 17 (PPU2 Pin 25)

#define SNES_CAPTURE_PIN_LAST PIN_SNES_HBLANK
#define SNES_CAPTURE_BITS     18

// =============================================================================
// Frequency Counter Pin Aliases (for debug tools)
// =============================================================================
#define PIN_FREQ_PCLK   PIN_SNES_PCLK
#define PIN_FREQ_R0     PIN_SNES_R0
#define PIN_FREQ_VBLANK PIN_SNES_VBLANK

#endif // SNES_PINS_H
