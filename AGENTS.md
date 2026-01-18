This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

superpico-digital is a digital-to-digital HDMI mod for 2-chip SNES consoles using the Raspberry Pi Pico 2 (RP2350). It captures native 15-bit digital RGB directly from the S-PPU2's TST pins—bypassing the console's analog video path entirely—and outputs clean HDMI via the RP2350's hardware HSTX peripheral. This approach eliminates the blur inherent in non-1CHIP SNES analog output.

## Build Commands

```bash
# Configure (first time or after CMakeLists.txt changes)
cd build && cmake .. -DPICO_SDK_PATH=$PICO_SDK_PATH

# Build
cd build && make -j8

# Flash to Pico 2
picotool load build/superpico-digital.uf2 -f && picotool reboot
```

Requires: Pico SDK, arm-none-eabi-gcc toolchain, picotool

## Architecture

### Hardware Target
- **MCU:** RP2350 (Pico 2) running at 126 MHz for HSTX timing
- **Video Output:** 640x480 @ 60Hz HDMI via GPIO 12-19 (HSTX peripheral)
  - GPIO 12-13: TMDS Clock
  - GPIO 14-15: TMDS Data 0
  - GPIO 16-17: TMDS Data 1
  - GPIO 18-19: TMDS Data 2

### Dual-Core Design
- **Core 0:** Main application logic, audio generation, animation updates
- **Core 1:** HSTX video output loop (launched via `multicore_launch_core1(video_output_core1_run)`)

### pico_hdmi Library Usage (lib/pico_hdmi)

Initialize in order:
```c
set_sys_clock_khz(126000, true);  // Required for HSTX timing
hstx_di_queue_init();             // Initialize data island queue
video_output_init(640, 480);      // Initialize video output
video_output_set_scanline_callback(callback);  // Register scanline renderer
multicore_launch_core1(video_output_core1_run);  // Start video on Core 1
```

Scanline callback signature:
```c
void scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst);
// dst: buffer for 640 RGB565 pixels packed as uint32_t pairs (320 words)
```

### SNES Digital Video Context

The project targets 2-chip SNES consoles (NOT 1-CHIP). Key signals from docs/:
- **PPU2 TST pins (77-92):** 15-bit digital RGB (5 bits per channel)
- **PPU1 /OVER (pin 94):** Mode 7 wrap detection - critical for transparency fix
- **PPU2 /TRANSPARENT (pin 4):** Valid pixel indicator
- **PPU2 /CSYNC (pin 100):** Composite sync

The TST pins output raw RGB before brightness is applied - brightness ($2100 register) must be implemented by snooping CPU writes and applying multiplication.

## Key Files

- `src/main.c` - Application entry point and rendering logic
- `lib/pico_hdmi/` - HDMI output library (git submodule)
- `docs/SNES_Digital_AV_Mod_Pin_Reference.md` - Complete SNES pinout reference
