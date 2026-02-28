# SNES Digital Capture: Technical Breakthroughs

This document tracks the key findings and breakthroughs discovered during the development of the `superpico-digital` mod, specifically regarding stable 15-bit digital RGB capture from the 2-chip SNES PPU2.

## 1. The "Hard Sync" Requirement
**Finding:** Relying on `CSYNC` (Composite Sync) for both horizontal and vertical timing is insufficient for a jitter-free digital mod.
**Why:** The SNES `CSYNC` signal contains equalization pulses and serration pulses during the vertical blanking interval. Attempting to distinguish these via software timers or PIO pulse counters introduces micro-jitter that causes the image to "wobble" or "scroll."
**Breakthrough:** Utilizing the dedicated **HBLANK (PPU2 Pin 25)** and **VBLANK (PPU2 Pin 26)** signals. These provide absolute, hardware-level "windowing" for the active video area.

## 2. The "Phase-Lock" Breakthrough (Fixing "Hot Air" Distortion)
**Finding:** Even with Hard Sync, the image exhibited a horizontal "shimmer" or "heat wave" effect.
**Why:** This was caused by the PIO's internal execution state being out of phase with the SNES Pixel Clock (PCLK). If the PIO starts waiting for PCLK *immediately* after HBLANK falls, the delay until the first rising edge varies by a few nanoseconds line-by-line.
**Breakthrough:** **Line-Start Phase Alignment.** Immediately after HBLANK goes LOW, the PIO explicitly waits for one full PCLK cycle (Rising then Falling) before entering the pixel capture loop. This "locks" the PIO's execution phase to the SNES clock for the duration of that line, ensuring 1:1 pixel alignment across the entire frame.

```pioasm
line_loop:
    wait 1 gpio 26             ; Wait for HBLANK HIGH
    wait 0 gpio 26             ; Wait for HBLANK LOW
    
    ; PHASE LOCK STEP
    wait 1 gpio 20             ; Wait for first PCLK Rising Edge
    wait 0 gpio 20             ; Wait for Falling Edge (Ready to sample next rising)
    
    ; Now start pixel capture...
```

## 3. Data Stability & Setup Time
**Finding:** Pixels appeared "noisy" or "corrupted" even when synchronized.
**Why:** The SNES TST pins are raw internal signals. Sampling at the exact moment of the PCLK rising edge is unstable because the data bits are in transition.
**Breakthrough:** **Post-Edge Setup Padding.** Adding a specific delay (`nop [2]`) after the PCLK rising edge before executing the `in pins` instruction. At 126MHz, this provides approximately **24ns** of setup time, allowing the data bus to stabilize.

## 4. Software/Hardware Synchronization
**Finding:** The image would occasionally "freeze" or "scrolling" would restart every few seconds.
**Why:** Core 0 (capture) and the PIO State Machine would occasionally get "out of phase," where the PIO was waiting for a frame that the C code thought was already happening.
**Breakthrough:** **C-Polled Frame Reset.** Core 0 explicitly polls the VBLANK GPIO edge and then sends an IRQ to "release" the PIO for a single frame. This hardware-software handshake ensures they are perfectly synchronized every 16.6ms (NTSC).

## 5. Bus Contention Management
**Finding:** Using a large (128+ line) ring buffer caused the LED heartbeat to slow down (2-4 seconds).
**Why:** On the RP2350, Core 0 and Core 1 share access to the RAM banks. A large buffer creates high contention between the DMA capture (Core 0) and the HDMI scanline output (Core 1).
**Breakthrough:** **Bank-Aligned Ping-Pong Buffers.** Using two small line buffers for hardware capture and a 256-line ring buffer for output, with careful attention to memory alignment, allows both cores to run at full speed without stalling.

## 6. Full 15-bit Color (RGB555 → RGB565)
**Finding:** The initial proof-of-concept only used 1 bit (R0) to display a monochrome image. Enabling all 15 color bits produced garbled colors.
**Why:** The PPU2 TST pins wire the MSB of each channel (R4/G4/B4) to the lower GPIO, so the 5-bit fields are bit-reversed in the captured word. This is the same pattern found in the Neo Geo MVS (neopico-hd project).
**Breakthrough:** **Pre-computed 32K LUT with per-channel bit-reversal.** A 64KB lookup table (`g_pixel_lut[32768]`) is generated at startup. Each entry maps a raw RGB555 index (with reversed channel bits) to a corrected RGB565 value. The hot pixel loop becomes a single table lookup: `g_pixel_lut[(raw >> 2) & 0x7FFF]`.

## 7. Horizontal Offset Calibration
**Finding:** The captured image was shifted right by ~5 pixels and clipped on the right edge.
**Why:** HBLANK deassertion does not coincide with the first valid pixel on the TST data bus. The PPU2 has an internal pipeline delay and back porch period of approximately 20 dot clocks between HBLANK falling and the first active pixel appearing.
**Breakthrough:** **Empirical PIO skip calibration.** A 20-pixel skip loop after HBLANK falls aligns the 256-pixel capture window with the true active area. This value was determined by binary search (0→24→20) and is analogous to `H_SKIP_START=28` in the neopico-hd MVS project.
