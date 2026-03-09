# SuperPico Digital - Session Scratchbook

## Hardware Pin Mapping (VERIFIED by user with scope)
- GP22 = S-DSP Pin 44 = SDATA (serial data)
- GP23 = S-DSP Pin 43 = LRCK (~32kHz word clock)
- GP24 = S-DSP Pin 42 = BCLK (~1.536MHz bit clock)
- Pin mapping is CONFIRMED CORRECT at solder joints (scope verified).

## Logic Analyzer Setup
- Dedicated LA Pico (separate from SuperPico), wired to S-DSP audio signals
- LA Pico wiring: GPIO2=BCLK, GPIO3=LRCK, GPIO4=SDATA
- LA firmware INPUT_PIN_BASE=2, so GPIO2=Ch1, GPIO3=Ch2, GPIO4=Ch3
- CLI: `dotnet ~/Projects/references/logicanalyzer/TerminalCapture.dll capture /dev/cu.usbmodem21301 <settings.tcs> <output.csv>`
- Created `/logic-analyzer` skill in `~/.claude/skills/` for reuse across sessions

## Logic Analyzer Findings (2026-02-28)
- **BCLK (Ch1/GPIO2)**: PERFECT. 1538 kHz, 50% duty, ±20ns jitter. Matches S-DSP spec exactly.
- **SDATA (Ch3/GPIO4)**: Looks like real data. Variable pulse widths, ~32% high. Consistent with serial audio.
- **LRCK (Ch2/GPIO3)**: BROKEN. Only 20ns glitch spikes (1 sample at 50MHz), not a real square wave.
  - Should be ~32 kHz, 50% duty (~15.6 us high, ~15.6 us low)
  - Actually seeing ~104 kHz random spikes, 0.2% duty cycle
  - BCLKs per LRCK "frame" wildly inconsistent: 21, 3, 41, 26, 3, 3, 41...
  - User confirmed scope also shows LRCK as "varying and all over the place"
- **Root cause of scratchy audio**: PIO `wait` instructions trigger on LRCK glitches, causing random bit-framing. L/R channels misaligned, audio values are garbage.
- **Next step**: User checking analog audio output to confirm SNES audio is good at the source. If analog is fine, LRCK solder joint / pin connection needs physical inspection.

## S-DSP Audio Format (from MiSTer DSP.vhd + uPD6376 datasheet + research)
- 16-bit signed PCM, right-justified in 24-bit frames
- MSB first
- ~32kHz sample rate (nominal 32000, typical ~32040 Hz)
- BCLK ~1.536 MHz (48 BCK per stereo frame, 24 per channel)
- LRCK HIGH = Left channel, LRCK LOW = Right channel
- S-DSP transitions data on falling edge of BCLK
- uPD6376 DAC latches on rising edge of BCLK
- Data is stable during BCLK HIGH period

## Bit Extraction
- PIO shifts left (MSB first), 24 bits captured
- After 24 `in` ops: ISR[23:16] = 8 padding bits, ISR[15:0] = 16-bit audio
- Correct extraction: `(int16_t)(raw & 0xFFFF)` — VERIFIED correct

## Architecture
- Core 0: Video capture loop
- Core 1: HDMI output + audio as background task via `video_output_set_background_task()`
- Audio uses PIO0 SM2 (SM0,1 used by freq_counter)
- DMA ring-wrapped 4096-word buffer, runs forever (count=0xFFFFFFFF)

## pico_hdmi Lib
- Updated to commit 5d8acde (same as neopico-hd) — this fixed major sync issues
- DO NOT use PICO_HDMI_RUNTIME_MODES (uses video_output.c, not video_output_rt.c)

## Critical Lessons Learned

### Things that BREAK video sync on Core 1:
- Removing `__scratch_x` from `audio_pipeline_process` — causes flash contention with HDMI
- Large memset operations (e.g., 16KB DMA buffer clear)
- Watchdog restart cycles (stop PIO + memset + restart) — removed
- `pico_hdmi_set_audio_sample_rate(32000)` — native 32kHz made sync worse
- Wrong init order in main.c — must match neopico-hd sequence

### Things that IMPROVE sync:
- Matching neopico-hd startup sequence with sleep_ms calls (1000, 500, 200, 100)
- Init order: hstx_di_queue_init → video_pipeline_init → video_output_init → audio_pipeline_init → video_capture_init → Core 1 launch
- video_capture_init() BEFORE Core 1 launch (not after)
- Using updated pico_hdmi lib (5d8acde)

### Audio quality history:
- First working audio: user could hear game audio, "higher pitch clear but lower ones scratchy"
- SRC DROP mode → LINEAR mode: "better but still very noisy"
- Native 32kHz (no SRC): "about the same, harder to sync" — REVERTED
- Falling edge PIO sampling: "audio is worse" — REVERTED to rising edge + nop
- Current state (rising edge + nop + SRC LINEAR + drift compensation): "bad, really bad"

### What has NOT been tried yet:
- Checking if PIO program instruction count exceeds PIO0 instruction memory
- Checking DMA buffer alignment and ring wrap behavior
- Testing with fixed LRCK connection

## Current Code State (2026-02-28)
- i2s_capture.pio: Rising edge sampling with nop hold margin, 24-bit frames, LRCK sync
- audio_pipeline.c: SRC LINEAR mode (32040→48000), drift compensation, while-loop drain
- main.c: Matched neopico-hd startup sequence with sleep_ms calls
- Sync: "better now" after startup sequence fix
- Audio: "bad, really bad"

## Next Steps
- User testing analog audio output to confirm SNES is producing good audio
- If analog audio is fine: fix LRCK physical connection (bad solder joint on S-DSP Pin 43?)
- Once LRCK is solid, re-capture with LA to verify clean 32kHz 50% duty signal
- Then re-test digital audio pipeline end-to-end

## Known Gap: Brightness / Fade ($2100 INIDISP) — 2026-03-08
- TST pins output raw RGB **before** the master brightness multiplier is applied.
- Games using fades (SMW level transitions, title screen, etc.) will appear at full brightness — no fade visible.
- Not yet implemented. Requires:
  - **Hardware**: 74HC4078 (8-input NOR gate) wired to PA0–PA7 to produce ADDR_MATCH (HIGH when address == $00).
  - **6 GPIOs**: /PWR (PPU2 pin 6), ADDR_MATCH (NOR output), D0–D3 (PPU2 pins 15–12).
  - **PIO SM**: Detects /PWR low + ADDR_MATCH high, latches D[3:0] as g_brightness (0–15).
  - **Software**: Apply (color * brightness) / 15 per channel, either via LUT rebuild or scanline callback.
- Action blocked on hardware feasibility (can the 74HC4078 be added to current layout?).
