# SuperPico Digital

Digital-to-digital HDMI mod for 2-chip SNES consoles using the Raspberry Pi Pico 2 (RP2350).

## Overview

SuperPico Digital captures native 15-bit digital RGB directly from the S-PPU2's TST pins - bypassing the console's analog video path entirely - and outputs clean HDMI via the RP2350's hardware HSTX peripheral. This approach eliminates the blur inherent in non - 1CHIP SNES consoles and provides a rock-solid, jitter-free image.

## Key Features

- **Hard Sync Architecture**: Uses physical `VBLANK` and `HBLANK` wires for perfect frame and line alignment.
- **Phase-Locked PIO Loop**: A custom PIO program synchronizes to the SNES Pixel Clock (`PCLK`) phase on every line, completely eliminating horizontal "hot air" jitter.
- **Jitter-Free 1:1 Capture**: Strategic setup delays ensure the raw SNES data bus is sampled at the peak of stability.
- **High Performance**: Leverages the RP2350 (Pico 2) HSTX peripheral for 640x480 @ 60Hz HDMI output.

## Hardware Requirements

- **Console**: 2-chip or 3-chip SNES (e.g., SNS-CPU-GPM-01/02, SHVC-CPU-01). **NOT compatible with 1-CHIP consoles.**
- **Microcontroller**: Raspberry Pi Pico 2 (RP2350).
- **Video Signals**: Requires soldering to PPU2 pins for digital RGB, Sync, and Clock.

## Wiring Reference

### Capture Pins (GP27-GP44 contiguous, PIO1 with GPIOBASE=16)

| Signal | SNES Source | Pico 2 GPIO | Capture Bit |
|--------|-------------|-------------|-------------|
| **VBLANK** | PPU2 Pin 26 | GP27 | 0 |
| **PCLK** | PPU2 Pin 27 | GP28 | 1 |
| **B4-B0** | PPU2 TST | GP29-33 | 2-6 |
| **G4-G0** | PPU2 TST | GP34-38 | 7-11 |
| **R4-R0** | PPU2 TST | GP39-43 | 12-16 |
| **HBLANK** | PPU2 Pin 25 | GP44 | 17 |

### HDMI Output

| Signal | Pico 2 GPIO |
|--------|-------------|
| **HDMI D0-D2** | GP14-19 |
| **HDMI CLK** | GP12-13 |

## Build & Flash

### Prerequisites
- Pico SDK
- `arm-none-eabi-gcc` toolchain
- `picotool`

### Build Commands
```bash
# Configure
mkdir build && cd build
cmake .. -DPICO_SDK_PATH=/path/to/pico-sdk

# Compile
make -j8

# Flash
picotool load src/superpico-digital.uf2 -f && picotool reboot
```

## Current Status

- [x] Stable Hardware Sync (HBLANK/VBLANK)
- [x] Jitter-free Pixel Capture
- [x] HDMI Signal Stability (640x480)
- [x] Horizontal Centering and Offset Fixes
- [x] Full 15-bit RGB555 Color (32K LUT with per-channel bit-reversal → RGB565)
- [ ] Digital Audio capture
- [ ] Master Brightness Logic ($2100) — 6 GPIOs + 74HC4078
- [ ] Mode 7 Transparency Patch

## Credits & References

- Based on logic from the **SNES_TST** FPGA project by Opatusos/michael-hirschmugl.
- Inspired by the **neopico-hd** project for MVS.
- Uses the **pico_hdmi** library for HSTX output.
