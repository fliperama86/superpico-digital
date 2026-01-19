# SuperPico Digital - Capture Debug Status

**Date:** 2026-01-18
**Status:** Work in Progress

## Overview

Debugging SNES digital video capture for 2-chip SNES consoles using RP2350 (Pico 2).

## Hardware Setup

- **MCU:** RP2350 (Pico 2) @ 126 MHz
- **Target:** 2-chip SNES (non-1CHIP)
- **Capture Pins:**
  - GP20: PCLK (Pixel Clock, ~5.37 MHz)
  - GP21: R0 (Red bit 0 - currently only bit being captured)
  - GP22: CSYNC (Composite Sync)

## Key Findings

### 1. CSYNC Detection - WORKING

CSYNC detection via PIO is fully functional:
- PCLK: ~40,350 toggles in 50k samples (ACTIVE)
- CSYNC: ~1,628 edges in 50ms (~32.5 kHz, matches NTSC h-rate × 2)
- HSYNC pulses: 316 PIO cycles (high time)
- Equalization pulses: ~157 cycles (half of HSYNC)
- VSYNC detection: Working via 8+ equalization pulse detection

### 2. VSYNC Detection - WORKING

State machine approach from neopico-hd:
1. Count short pulses (equalization)
2. When 8+ short pulses followed by long pulse → entering VSYNC
3. Wait for next long pulse → VSYNC END, frame starts

### 3. Pixel Capture via PIO + DMA - WORKING

- PIO captures 3 bits per pixel: [CSYNC, R0, PCLK]
- DMA transfers SNES_H_TOTAL (341) words per line
- Successfully captures 224 active lines per frame
- Frame capture via serial confirmed working (captured SMW overworld)

### 4. Serial Frame Transfer - WORKING

Protocol:
1. Send 'C' to trigger capture
2. Wait for "FRAME_START:256:224"
3. Receive 7168 bytes (256×224 pixels, 1-bit packed)
4. Wait for "FRAME_END"

Python script: `scripts/capture_frame.py`

### 5. HDMI Output - PARTIALLY WORKING

- HDMI initialization was hanging when capture was initialized first
- **Solution:** Initialize HDMI before capture, don't wait for USB connection
- HDMI now displays captured frames in real-time
- Frame is stable (no vertical bouncing after removing debug printf)

## Resolved: Vertical Duplication (was visual artifact)

The captured/displayed image appeared to show duplicated content vertically.

### Resolution:
- Debug markers (R/G/B at lines 0/112/223) appeared correctly - NOT duplicated
- **Linear gradient test CONFIRMED: gradient goes black→white ONCE (correct!)**
- The "duplication" was a visual artifact of 1-bit capture on repetitive game graphics
- SMW overworld map has naturally repetitive patterns that look similar at 1-bit depth

### Conclusion:
**Capture pipeline is working correctly.** The apparent duplication was due to the loss of detail when capturing only R0 (1-bit), not an actual bug.

## Timing Constants

From `src/video/snes_timing.h`:
```c
#define SNES_H_TOTAL      341   // Total dots per line
#define SNES_H_ACTIVE     256   // Active pixels per line
#define SNES_V_TOTAL      262   // Total lines per frame (NTSC)
#define SNES_V_ACTIVE     224   // Active lines
#define SNES_H_SKIP_START 48    // Horizontal offset to active area
#define SNES_V_SKIP_LINES 16    // Vertical blanking lines to skip
#define H_THRESHOLD       255   // Pulse width threshold for VSYNC detection
```

## Code Architecture

### Core 0 (Capture):
1. `wait_for_vsync()` - Detects VSYNC via CSYNC pulse analysis
2. Reset pixel PIO state machine
3. Skip V_SKIP_LINES via DMA (discard vblank)
4. Capture 224 active lines via DMA
5. Convert R0 bit to RGB565 and write to framebuffer

### Core 1 (HDMI Output):
1. `video_output_core1_run()` - Runs HDMI output loop
2. `scanline_callback()` - Called per output line
3. 2× horizontal scaling (256 → 512, centered in 640)
4. 2× vertical scaling (224 → 448, centered in 480)

### PIO Programs (`video_capture.pio`):

**snes_sync** - Measures CSYNC high time:
- Waits for CSYNC high
- Counts PCLK edges until CSYNC low
- Pushes count to FIFO

**snes_pixel_capture** - Captures pixel data:
- Waits for IRQ 4 trigger
- Syncs to CSYNC edges (line start)
- Samples 3 bits on each PCLK rising edge
- Captures SNES_H_TOTAL samples per line

## Next Steps

1. **Verify duplication issue** - Complete linear gradient test
2. **Add more color bits** - Capture R1-R4, G0-G4, B0-B4 for full 15-bit
3. **Optimize capture** - Consider ring buffer approach from neopico-hd
4. **Investigate HDMI hang** - Understand why HDMI init hangs after capture init

## Files Modified

- `src/main.c` - Main initialization and loop
- `src/video/video_capture.c` - Capture implementation
- `src/video/video_capture.pio` - PIO programs for sync and pixel capture
- `src/video/snes_timing.h` - Timing constants
- `src/snes_pins.h` - GPIO pin definitions
- `scripts/capture_frame.py` - Serial frame capture tool
- `scripts/monitor_serial.py` - Serial monitor tool

## Useful Commands

```bash
# Build
cd build && make -j8

# Flash (with auto-reboot to bootloader)
picotool reboot -fu && sleep 2 && picotool load build/src/superpico-digital.uf2 && picotool reboot

# Monitor serial
python3 scripts/monitor_serial.py

# Capture frame
python3 scripts/capture_frame.py output.png
```

## References

- neopico-hd project: Similar architecture for Neo Geo MVS capture
- SNES dev wiki: PPU timing documentation
- `docs/SNES_Digital_AV_Mod_Pin_Reference.md`: Pin reference for SNES PPU
