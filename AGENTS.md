# AI Agent Instructions - superpico-digital

This file provides specific guidance for AI agents (Claude Code, etc.) working on this repository.

## CRITICAL RULES
- **DO NOT** modify the content of the `lib/` folder (especially `pico_hdmi`). It is a submodule managed externally.
- **DO NOT** change HSTX timing constants without explicit approval; the RP2350 requires precise PLL settings for HDMI.
- **ALWAYS** perform tool calls in parallel when possible.

## Architecture & Implementation Details

### Dual-Core Design
- **Core 0**: Handles the high-level capture loop. It polls the `VBLANK` GPIO falling edge to identify the start of an active frame and then signals the PIO State Machine via IRQ 4.
- **Core 1**: Dedicated to HDMI output. Runs the `video_output_core1_run` loop from `libpico_hdmi`.

### Video Capture Strategy (Hard Sync)
The capture logic is defined in `src/video/video_capture.pio`.
1. **Phase Locking**: The PIO waits for a full PCLK cycle (`wait 1` -> `wait 0`) after `HBLANK` falls. This eliminates horizontal jitter by ensuring the capture loop starts at the exact same phase relative to the SNES clock on every line.
2. **Setup Delay**: A `nop [2]` (approx 24ns) is placed after the PCLK rising edge to allow the raw data bits (TST pins) to stabilize before sampling.
3. **Horizontal Offset**: A 12-pixel skip loop is implemented in PIO to align the 256-pixel window with the actual active video area of the SNES.

### Timing Constants
- **System Clock**: 126 MHz (required for HSTX HDMI timing).
- **SNES Resolution**: 256x224 (NTSC).
- **HDMI Resolution**: 640x480 @ 60Hz.

### Memory Layout
- **Ring Buffer**: `line_ring_t` in `src/video/line_ring.h` manages the handover between Core 0 and Core 1.
- **DMA**: Uses a ping-pong buffer strategy (`g_line_buffers[2]`) to ensure the PIO/DMA can capture the next line while the CPU is processing the current one.

## Key Files
- `src/main.c`: Entry point and core sync logic.
- `src/video/video_capture.c`: DMA and state machine management.
- `src/video/video_capture.pio`: Low-level timing and sync implementation.
- `src/video/video_pipeline.c`: Scanline callback and scaling logic.
- `docs/CAPTURE_BREAKTHROUGH.md`: Detailed history of signal discovery and jitter fixes.
