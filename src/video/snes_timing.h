#ifndef SNES_TIMING_H
#define SNES_TIMING_H

// =============================================================================
// SNES NTSC Timing Constants
// =============================================================================

#define SNES_H_TOTAL      341
#define SNES_H_ACTIVE     256
#define SNES_V_TOTAL      262
#define SNES_V_ACTIVE     224

// Capture tuning (relative to CSYNC falling edge)
// These values determine which part of the horizontal line is captured.
// Based on 341 total dots, 256 active. 
// Blanking = 85 dots.
#define SNES_H_SKIP_START 48  // Back porch + Sync duration
#define SNES_H_SKIP_END   37  // Front porch
#define SNES_V_SKIP_LINES 16  // Vertical blanking lines before active video

// PIO Sync Detection
// Threshold to distinguish HSYNC from VSYNC/EQ pulses
// SNES H-Sync is ~4.7us. 5.37MHz * 4.7us = ~25 dots.
// The counter in PIO usually runs at sys_clk or a divider.
// If sys_clk = 126MHz, 4.7us = 592 cycles.
// We use a threshold roughly 75% of a full line (341 dots).
#define H_THRESHOLD       255

#endif // SNES_TIMING_H