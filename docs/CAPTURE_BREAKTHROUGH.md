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
