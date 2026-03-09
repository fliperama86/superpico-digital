# SNES Digital AV Mod - Complete Pin Reference

> **Project:** SuperPico Digital (RP2350B + HDMI via HSTX)  
> **Applicable Hardware:** 2-chip/3-chip SNES (NOT 1-CHIP)  
> **Last Updated:** March 2026

---

## Table of Contents

1. [Overview](#overview)
2. [S-PPU2 (5C78) - Primary Digital Video Source](#s-ppu2-5c78---primary-digital-video-source)
3. [S-PPU1 (5C77) - Control Signals Source](#s-ppu1-5c77---control-signals-source)
4. [S-DSP - Digital Audio Source](#s-dsp---digital-audio-source)
5. [CIC/SuperCIC - Region and Reset Control](#cicsupercic---region-and-reset-control)
6. [Implementation Notes](#implementation-notes)
7. [Signal Timing and Clocks](#signal-timing-and-clocks)
8. [Known Issues and Solutions](#known-issues-and-solutions)
9. [References](REFERENCES.md)

---

## Overview

This document provides a complete reference for all pins relevant to implementing a digital AV mod for 2-chip/3-chip SNES consoles. The mod extracts native 15-bit digital RGB from the S-PPU2's test pins, bypassing the console's analog video path which introduces blur on non-1CHIP models.

### Why 2-Chip/3-Chip Only?

The 1-CHIP SNES (S-CPUN) consolidates PPU1 and PPU2 into a single die. While it has TST pins, it does **not expose the blanking signals** (OVER, TOUMEI) needed to properly handle Mode 7 transparency and other edge cases. Only the discrete 2-chip PPU2 exposes these critical control signals.

---

## S-PPU2 (5C78) - Primary Digital Video Source

The S-PPU2 (Nintendo 5C78) is a QFP-100 package with 0.65mm pitch. This chip contains the CGRAM (palette), performs color math, and generates the final analog RGB output. When DIGITAL VIDEO ENABLE is asserted, digital RGB is available on the TST pins.

### Complete S-PPU2 Pinout

```
                                      _____
                                     /     \
                          /BURST <- / 1 100 \ -> /CSYNC
                           /PED <- / 2    99 \ -- GND
                COLORBURST CLK <- / 3      98 \ <- HVCMODE
                 /TRANSPARENT <> / 4        97 \ -> B (Analog Blue)
                         +5V -- / 5          96 \ -> G (Analog Green)
                       /PWR -> / 6            95 \ -> R (Analog Red)
                      /PRD -> / 7              94 \ -- +5VA
                   CPU D7 <> / 8                93 \ <- DIGITAL VIDEO ENABLE
                  CPU D6 <> / 9                  92 \ <> TST14
                 CPU D5 <> / 10                   91 \ <> TST13
                CPU D4 <> / 11                     90 \ <> TST12
               CPU D3 <> / 12                       89 \ <> TST11
              CPU D2 <> / 13                         88 \ <> TST10
             CPU D1 <> / 14                           87 \ <> TST9
            CPU D0 <> / 15                             86 \ <> TST8
              GND -- / 16                               85 \ <> TST7
             PA7 -> / 17                                 84 \ <> TST6
            PA6 -> / 18                                   83 \ -- +5V
           PA5 -> / 19                                     82 \ <> TST5
          PA4 -> / 20                                       81 \ <> TST4
         PA3 -> / 21                                            \
        PA2 -> / 22                                     O       /
       PA1 -> / 23                                          80 / <> TST3
      PA0 -> / 24                                          79 / <> TST2
  HBLANK <- / 25                                          78 / <> TST1
 VBLANK <- / 26              Nintendo 5C78               77 / <> TST0
/PIXEL CLK OUT <- / 27      Package QFP-100             76 / <- EXT7
 /RESOUT1 <- / 28           0.65mm pitch               75 / <- EXT6
/EXTLATCH -> / 29                                      74 / <- EXT5
PALMODE -> / 30              S-PPU2                   73 / <- EXT4
          /       O                                  72 / <- EXT3
          \                                         71 / <- EXT2
SYSTEM CLK -> \ 31                                 70 / <- EXT1
     +5V -- \ 32                                  69 / <- EXT0
 /RESOUT0 <- \ 33                                68 / -- GND
    /RESET -> \ 34                              67 / <- VDA7
        GND -- \ 35                            66 / <- VDA6
       FIELD -> \ 36                          65 / <- VDA5
       /OVER1 -> \ 37                        64 / <- VDA4
 /PIXEL CLK IN -> \ 38                      63 / <- VDA3
          /HCLD -> \ 39                    62 / <- VDA2
           /VCLD -> \ 40                  61 / <- VDA1
           COLOR0 -> \ 41                60 / <- VDA0
            COLOR1 -> \ 42              59 / -- +5V
             COLOR2 -> \ 43            58 / <- VDB7
               PRIO0 -> \ 44          57 / <- VDB6
                PRIO1 -> \ 45        56 / <- VDB5
                  CHR0 -> \ 46      55 / <- VDB4
                   CHR1 -> \ 47    54 / <- VDB3
                    CHR2 -> \ 48  53 / <- VDB2
                     CHR3 -> \ 49 52 / <- VDB1
                    /OVER2 -> \ 50 51 / <- VDB0
                               \     /
                                \   /
                                 \ /
                                  V
```

### Digital Video Pins (Active when DIGITAL VIDEO ENABLE = HIGH)


| Pin    | Name                     | Direction    | Function                                                                                              |
| ------ | ------------------------ | ------------ | ----------------------------------------------------------------------------------------------------- |
| **93** | **DIGITAL VIDEO ENABLE** | Input        | Master enable for digital RGB output. Connect to /OVER for proper blanking. Factory-connected to GND. |
| 77     | TST0                     | I/O → Output | Red bit 0 (LSB)                                                                                       |
| 78     | TST1                     | I/O → Output | Red bit 1                                                                                             |
| 79     | TST2                     | I/O → Output | Red bit 2                                                                                             |
| 80     | TST3                     | I/O → Output | Red bit 3                                                                                             |
| 81     | TST4                     | I/O → Output | Red bit 4 (MSB)                                                                                       |
| 82     | TST5                     | I/O → Output | Green bit 0 (LSB)                                                                                     |
| 84     | TST6                     | I/O → Output | Green bit 1                                                                                           |
| 85     | TST7                     | I/O → Output | Green bit 2                                                                                           |
| 86     | TST8                     | I/O → Output | Green bit 3                                                                                           |
| 87     | TST9                     | I/O → Output | Green bit 4 (MSB)                                                                                     |
| 88     | TST10                    | I/O → Output | Blue bit 0 (LSB)                                                                                      |
| 89     | TST11                    | I/O → Output | Blue bit 1                                                                                            |
| 90     | TST12                    | I/O → Output | Blue bit 2                                                                                            |
| 91     | TST13                    | I/O → Output | Blue bit 3                                                                                            |
| 92     | TST14                    | I/O → Output | Blue bit 4 (MSB)                                                                                      |


**Note:** TST14-12 control test mode selection when DIGITAL VIDEO ENABLE is LOW. Factory-connected to GND.

### Synchronization and Timing Signals


| Pin     | Name               | Direction | Function                                                            |
| ------- | ------------------ | --------- | ------------------------------------------------------------------- |
| **100** | **/CSYNC**         | Output    | Composite sync output. Active low.                                  |
| **1**   | **/BURST**         | Output    | Color burst gate signal. Can be used as alternative sync reference. |
| **27**  | **/PIXEL CLK OUT** | Output    | 5.37 MHz dot clock output. Goes to expansion port pin 22.           |
| **38**  | **/PIXEL CLK IN**  | Input     | 5.37 MHz dot clock input from S-PPU1.                               |
| **31**  | **SYSTEM CLK**     | Input     | 21.47727 MHz master clock (NTSC).                                   |
| **3**   | **COLORBURST CLK** | Input     | 3.58 MHz color subcarrier clock.                                    |


### Blanking and Control Signals


| Pin    | Name                      | Direction     | Function                                                                          |
| ------ | ------------------------- | ------------- | --------------------------------------------------------------------------------- |
| **25** | **HBLANK**                | Output        | Horizontal blanking signal.                                                       |
| **26** | **VBLANK**                | Output        | Vertical blanking signal.                                                         |
| **4**  | **/TRANSPARENT (TOUMEI)** | Bidirectional | High when opaque pixel drawn; low during sync/burst. **Critical for Mode 7 fix.** |
| **37** | **/OVER1**                | Input         | Overflow/wrap signal from S-PPU1. Connect to pin 94 of PPU1.                      |
| **50** | **/OVER2**                | Input         | Second overflow input from S-PPU1. Connect to pin 94 of PPU1.                     |
| **36** | **FIELD**                 | Input         | Interlace field indicator from PPU1.                                              |
| **39** | **/HCLD**                 | Input         | Horizontal counter load from PPU1.                                                |
| **40** | **/VCLD**                 | Input         | Vertical counter load from PPU1.                                                  |


### Analog Video Output (For Reference/Hybrid Mods)


| Pin | Name | Direction | Function                              |
| --- | ---- | --------- | ------------------------------------- |
| 95  | R    | Output    | Analog red output (0-700mV typical)   |
| 96  | G    | Output    | Analog green output (0-700mV typical) |
| 97  | B    | Output    | Analog blue output (0-700mV typical)  |
| 94  | +5VA | Power     | Analog power supply for video DACs    |


### PPU1-PPU2 Communication (Pixel Data from PPU1)


| Pin   | Name      | Direction | Function                    |
| ----- | --------- | --------- | --------------------------- |
| 41    | COLOR0    | Input     | Color index bit 0 from PPU1 |
| 42    | COLOR1    | Input     | Color index bit 1 from PPU1 |
| 43    | COLOR2    | Input     | Color index bit 2 from PPU1 |
| 44    | PRIO0     | Input     | Priority bit 0 from PPU1    |
| 45    | PRIO1     | Input     | Priority bit 1 from PPU1    |
| 46-49 | CHR0-CHR3 | Input     | Character data from PPU1    |


### External Video Input (EXT Pins)


| Pin   | Name      | Direction | Function                                                                    |
| ----- | --------- | --------- | --------------------------------------------------------------------------- |
| 69-76 | EXT0-EXT7 | Input     | External video input, connected to VDB7..0. Used for external video mixing. |


### Reset and Mode Configuration


| Pin | Name      | Direction | Function                                                    |
| --- | --------- | --------- | ----------------------------------------------------------- |
| 28  | /RESOUT1  | Output    | Main reset output to CPU, APU, cartridge, expansion port.   |
| 33  | /RESOUT0  | Output    | Reset output to S-PPU1.                                     |
| 34  | /RESET    | Input     | Reset input from CIC.                                       |
| 29  | /EXTLATCH | Input     | H/V counter latch control. Connected to joypad IO D7.       |
| 30  | PALMODE   | Input     | PAL/NTSC mode select. High = PAL (50Hz), Low = NTSC (60Hz). |
| 98  | HVCMODE   | Input     | H/V counter mode configuration.                             |


### CPU Interface


| Pin   | Name      | Direction     | Function                      |
| ----- | --------- | ------------- | ----------------------------- |
| 8-15  | CPU D7-D0 | Bidirectional | 8-bit CPU data bus            |
| 17-24 | PA7-PA0   | Input         | 8-bit PPU address bus (B-bus) |
| 6     | /PWR      | Input         | CPU write strobe              |
| 7     | /PRD      | Input         | CPU read strobe               |


### VRAM Data Interface


| Pin   | Name      | Direction | Function        |
| ----- | --------- | --------- | --------------- |
| 51-58 | VDB0-VDB7 | Input     | VRAM data bus B |
| 60-67 | VDA0-VDA7 | Input     | VRAM data bus A |


---

## S-PPU1 (5C77) - Control Signals Source

The S-PPU1 (Nintendo 5C77) is also a QFP-100 package with 0.65mm pitch. It handles sprite/background rendering, VRAM addressing, and generates control signals needed by PPU2.

### Critical Control Signals for Digital Video Mod


| Pin    | Name           | Direction     | Function                                                                                                                                                                       |
| ------ | -------------- | ------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **94** | **/OVER**      | Output        | **Mode 7 overflow/wrap indicator.** Connect to PPU2 pins 37 and 50, and to DIGITAL VIDEO ENABLE (pin 93) via logic. When low, indicates pixel is outside the Mode 7 playfield. |
| 93     | /PIXEL CLK OUT | Output        | 5.37 MHz pixel clock output to PPU2.                                                                                                                                           |
| 95     | FIELD          | Bidirectional | Interlace field signal, shared with PPU2.                                                                                                                                      |
| 92     | /HCLD          | Bidirectional | Horizontal counter load.                                                                                                                                                       |
| 91     | /VCLD          | Bidirectional | Vertical counter load.                                                                                                                                                         |


### Complete S-PPU1 Pinout

```
                                     ^
                                    / \
                                   /   \
                                  /     \
                         TST1 -> / 1 100 \ <- SYSTEM CLK
                        TST0 -> / 2    99 \ <- TST2
                       /PRD -> / 3  (*) 98 \ <- /RESET
                      /PWR -> / 4        97 \ <- /PIXEL CLK IN
                      PA7 -> / 5          96 \ -- GND
                     PA6 -> / 6            95 \ <> FIELD
                    PA5 -> / 7              94 \ -> /OVER
                   PA4 -> / 8                93 \ -> /PIXEL CLK OUT
                  PA3 -> / 9                  92 \ <> /HCLD
                 PA2 -> / 10                   91 \ <> /VCLD
                PA1 -> / 11                     90 \ -> COLOR0
               PA0 -> / 12                       89 \ -> COLOR1
              +5V -- / 13                         88 \ -> COLOR2
          CPU D7 <> / 14                           87 \ -> PRIO0
         CPU D6 <> / 15                             86 \ -> PRIO1
        CPU D5 <> / 16                               85 \ -> CHR0
       CPU D4 <> / 17                                 84 \ -> CHR1
      CPU D3 <> / 18                                   83 \ -> CHR2
     CPU D2 <> / 19                                     82 \ -> CHR3
    CPU D1 <> / 20                                       81 \ -- +5V
   CPU D0 <> / 21                                            \
     GND -- / 22                                             /
HVCMODE -> / 23                                          80 / -> /VRD
PALMODE -> / 24                                         79 / -> /VBWR
/MASTER -> / 25                                        78 / -> /VAWR
/EXTSYNC -> / 26              Nintendo 5C77           77 / -- GND
    GND -- / 27      Package QFP-100, 0.65mm pitch   76 / -> VAA0
  VDB0 <> / 28                                       75 / -> VAA1
 VDB1 <> / 29                  S-PPU1               74 / -> VAA2
VDB2 <> / 30                                       73 / -> VAA3
       /                                          72 / -> VAA4
       \                                         71 / -> VAA5
VDB3 <> \ 31                                    70 / -> VAA6
 VDB4 <> \ 32                                  69 / -> VAA7
  VDB5 <> \ 33                                68 / -> VAA8
   VDB6 <> \ 34                              67 / -> VAA9
    VDB7 <> \ 35                            66 / -> VAA10
      +5V -- \ 36                          65 / -> VAA11
      VDA0 <> \ 37                        64 / -> VAA12
       VDA1 <> \ 38                      63 / -> VAA13
        VDA2 <> \ 39                    62 / -- +5V
         VDA3 <> \ 40                  61 / -> VAB0
          VDA4 <> \ 41                60 / -> VAB1
           VDA5 <> \ 42              59 / -> VAB2
            VDA6 <> \ 43            58 / -> VAB3
             VDA7 <> \ 44          57 / -> VAB4
               GND -- \ 45        56 / -> VAB5
               VA15 <- \ 46      55 / -> VAB6
                VA14 <- \ 47    54 / -> VAB7
                VAB13 <- \ 48  53 / -> VAB8
                 VAB12 <- \ 49 52 / -> VAB9
                  VAB11 <- \ 50 51 / -> VAB10
                            \     /
                             \   /
                              \ /
                               V
```

### PPU1 Region/Mode Configuration


| Pin    | Name        | Direction | Function                                                    |
| ------ | ----------- | --------- | ----------------------------------------------------------- |
| **24** | **PALMODE** | Input     | PAL/NTSC mode. Must match PPU2 pin 30 for region switching. |
| 23     | HVCMODE     | Input     | H/V counter mode.                                           |
| 25     | /MASTER     | Input     | Master/slave mode. Normally grounded.                       |
| 26     | /EXTSYNC    | Input     | External sync input.                                        |


---

## S-DSP - Digital Audio Source

The S-DSP outputs serial digital audio that can be converted to S/PDIF for pristine audio extraction. The audio format is 32kHz, 16-bit stereo.

### S-DSP Digital Audio Pins (QFP-80, 0.8mm pitch)


| Pin    | Name                          | Direction | Function                                                      |
| ------ | ----------------------------- | --------- | ------------------------------------------------------------- |
| **44** | **DAC DATA (SDATA)**          | Output    | Serial audio data. 16-bit samples, MSB first.                 |
| **43** | **DAC LEFT/RIGHT CLK (LRCK)** | Output    | Word select / Left-Right clock. ~32kHz (actually ~32.040kHz). |
| **42** | **DAC BIT CLK (BCLK)**        | Output    | Bit clock for serial data.                                    |
| 46     | CRYSTAL IN                    | Input     | 24.576MHz crystal input.                                      |
| 45     | CRYSTAL OUT                   | Output    | Crystal oscillator output.                                    |
| 47     | /RESET                        | Input     | Reset input.                                                  |
| 39     | /MUTE                         | Output    | Mute control output to DAC.                                   |


### S-DSP Complete Pinout (Relevant Subset)

```
                    / 21 Nintendo S-DSP  62 / <- SMP A2
     DSP RAM D3 <> / 22  Package QFP-80  61 / <- SMP A1
     DSP RAM D4 <> / 23  0.8mm pitch     60 / <- SMP A0
     DSP RAM D5 <> / 24                  59 / <- SMP D7
                  /                      58 / <- SMP D6
                  \                      57 / <- SMP D5
     DSP RAM D6 <> \ 25                  56 / <- SMP D4
     DSP RAM D7 <> \ 26                  55 / <- SMP D3
    DSP RAM /CE1 <- \ 27                 54 / <- SMP D2
    DSP RAM /CE0 <- \ 28                 53 / <- SMP D1
    DSP RAM A10 <- \ 29                  52 / -- GND
     DSP RAM /OE <- \ 30                 51 / <- SMP D0
    DSP RAM A11 <- \ 31                  50 / -> SMP CLKEN
     DSP RAM A9 <- \ 32                  49 / <- SMP R/W
            +5V -- \ 33                  48 / -> SMP CLK
     DSP RAM A8 <- \ 34                  47 / <- /RESET
    DSP RAM A13 <- \ 35                  46 / <- CRYSTAL IN
     DSP RAM /WE <- \ 36       O         45 / -> CRYSTAL OUT
             TF -> \ 37                  44 / -> DAC DATA
             TK -> \ 38                  43 / -> DAC LEFT/RIGHT CLK
          /MUTE <- \ 39                  42 / -> DAC BIT CLK
                <- \ 40                  41 / -> CIC CLK
                    \                   /
                     \ _ _ _ _ _ _ _ _ /
```

### Digital Audio Implementation Notes

**SuperPico approach:** SDATA, LRCK, and BCLK are captured directly by the RP2350B via PIO/DMA, upsampled from ~32kHz to 48kHz, and embedded into the HDMI stream via data island packets. No external S/PDIF encoder IC is required.

For standalone S/PDIF output (not used in SuperPico), use a chip like the Cirrus Logic CS8405A or Texas Instruments DIT4096:

| S-DSP Pin      | S/PDIF Encoder Pin | Notes                   |
| -------------- | ------------------ | ----------------------- |
| Pin 44 (SDATA) | SDIN               | Serial data input       |
| Pin 43 (LRCK)  | LRCK/FSYNC         | Word clock / Frame sync |
| Pin 42 (BCLK)  | SCLK               | Serial clock            |

**Important:** The S-DSP outputs at approximately 32.040kHz (not exactly 32kHz) due to the 24.576MHz crystal. SuperPico's SRC (sample rate converter) compensates for this in firmware.

---

## CIC/SuperCIC - Region and Reset Control

The CIC (Checking Integrated Circuit) handles region lockout. For a complete AV mod with region-free capability, SuperCIC replacement is recommended.

### Original CIC Lock Chip (18-pin, in console)


| Pin | Name             | Function                                           |
| --- | ---------------- | -------------------------------------------------- |
| 1   | D2               | Data line 2 to cartridge CIC                       |
| 2   | D1               | Data line 1 to cartridge CIC                       |
| 3   | SEED             | Seed input                                         |
| 4   | ??               | Mode select (lift for region bypass on F41x chips) |
| 7   | CLK              | Clock input from S-CLK or cart slot pin 56         |
| 8   | RST              | Reset input                                        |
| 10  | /RESET (Cart)    | Reset output to cartridge                          |
| 11  | /RESET (Console) | Reset output to console                            |


### SuperCIC Integration Points


| Signal          | PPU Connection           | Function                      |
| --------------- | ------------------------ | ----------------------------- |
| PALMODE output  | PPU1 pin 24, PPU2 pin 30 | Sets video timing (50Hz/60Hz) |
| D4 patch output | PPU2 CPU D4 (pin 11)     | Region patch for $213F reads  |
| Clock input     | CIC pad 7                | SuperCIC clock source         |
| Reset output    | System reset line        | Directly replaces CIC reset   |


### D4 Region Patch

The D4 patch overrides bit 4 of PPU2's $213F status register response, which reports PAL/NTSC mode to the CPU. This is necessary because some games check this register for region verification.

---

## Implementation Notes

### Brightness Control ($2100)

The TST pins output raw 5-bit RGB **before** the master brightness multiplier is applied. Register `$2100` (INIDISP) bits 3–0 control brightness (0=black, 15=full). Games using fades will appear at full brightness without correction.

#### Brightness Update Timing

The brightness value is not fixed for a whole frame — the CPU can write `$2100` at any time:

| Granularity | Mechanism | Examples |
| ----------- | --------- | -------- |
| Per-frame | CPU writes during VBLANK | SMW level fades, title screen fade-in/out, Super Metroid |
| Per-scanline | HDMA writes `$2100` each HBLANK | Raster brightness effects (rare) |
| Mid-scanline | Direct CPU write outside blanking | Almost never done in retail software; causes hardware glitches |

**Recommended sampling rate: once per scanline (at HBLANK).** This correctly handles both frame-level fades and any per-line HDMA raster effects. Sampling only once per frame would miss scanline-based brightness changes.

The single-dot "Early Read Glitch" (a hardware bug where PPU2 briefly latches the wrong brightness for ~1 pixel during a mid-scanline write) does not need to be emulated — no retail game intentionally triggers it.

#### Hardware Solution (QSB approach)

Brightness detection and latching is handled entirely on the QSB daughter board, delivering 4 stable GPIO lines to the RP2350 that always hold the last value written to `$2100`.

| QSB Component | Function |
| ------------- | -------- |
| 8-input NOR gate (PA0–PA7) | Produces `ADDR_MATCH` — HIGH only when B-bus address == `$00` |
| AND gate (`ADDR_MATCH` + `/PWR`) | Produces `WRITE_2100` pulse when CPU writes to `$2100` |
| 74HC175 (4-bit latch) | Clocked by `WRITE_2100`; holds D[3:0] stable on output |

PPU2 pins tapped:

| Signal | PPU2 Pin | Notes |
| ------ | -------- | ----- |
| `/PWR` | 6 | CPU write strobe, active low |
| PA0–PA7 | 17–24 | B-bus address (into NOR gate on QSB) |
| D0 | 15 | Brightness bit 0 (LSB) |
| D1 | 14 | Brightness bit 1 |
| D2 | 13 | Brightness bit 2 |
| D3 | 12 | Brightness bit 3 (MSB) |

**FPC to RP2350:** 4 lines only (`Brightness[3:0]`). No timing-critical bus snooping needed on the main board.

#### Software

Read `Brightness[3:0]` from GPIOs once per scanline at HBLANK. Apply to each pixel:

```c
output_channel = (raw_channel * brightness) / 15;
```

Or pre-compute a per-brightness LUT to eliminate the multiply from the hot path.

**Note on D7 (force-blank):** D7 forces the screen black regardless of the brightness nibble, but is almost exclusively used during VBLANK for safe VRAM access. It is rarely asserted mid-frame in retail software. D[3:0] alone covers all practical fade effects; D7 can be omitted if GPIOs are scarce.

### Mode 7 Transparency Fix

When Mode 7 wraps around (pixels outside the 128×128 tile map with transparency mode enabled), /OVER from PPU1 goes low. The digital output continues showing stale data instead of transparency.

**SuperPico / QSB approach:** `/OVER` (PPU1 pin 94) and `/TRANSPARENT` (PPU2 pin 4) are combined into a single `PIXEL_VALID` signal on the QSB daughter board using an AND gate. The RP2350 reads one GPIO; when LOW, the pixel is suppressed (output color 0). See Appendix B for the full QSB signal flow.

### TOUMEI Signal Usage

The /TRANSPARENT signal (PPU2 pin 4) is HIGH during valid picture output and LOW during sync/burst periods. In SuperPico this signal is ANDed with /OVER on the QSB to produce `PIXEL_VALID`, which:

- Gates the digital video output
- Fixes Super Mario World title screen issues
- Properly blanks during horizontal/vertical sync

### SuperPico QSB Taps Summary

Beyond the core capture wiring (TST0–14, HBLANK, VBLANK, PCLK), these signals are tapped at the SNES and preprocessed on the QSB daughter board before reaching the RP2350:

| Signal on QSB | SNES Source | Delivered to RP2350 as | Feature |
| ------------- | ----------- | ---------------------- | ------- |
| `/OVER` + `/TRANSPARENT` | PPU1 p94 + PPU2 p4 | `PIXEL_VALID` (1 GPIO, ANDed on QSB) | Mode 7 + pixel blanking |
| PA0–PA7 → NOR → `/PWR` AND → latch | PPU2 p17–24, p6, p12–15 | `Brightness[3:0]` (4 GPIOs, latched on QSB) | Brightness ($2100) |
| PALMODE | PPU2 p30 | `PALMODE` (1 GPIO, pass-through) | PAL/NTSC detection |


---

## Signal Timing and Clocks

### Clock Frequencies


| Clock             | NTSC          | PAL            | Source             |
| ----------------- | ------------- | -------------- | ------------------ |
| Master Clock      | 21.47727 MHz  | 21.28137 MHz   | Crystal oscillator |
| Dot Clock         | 5.3693175 MHz | 5.3203425 MHz  | Master ÷ 4         |
| Color Burst       | 3.579545 MHz  | 4.43361875 MHz | Subcarrier         |
| Audio Sample Rate | ~32.040 kHz   | ~32.040 kHz    | S-DSP crystal      |


### Video Timing


| Parameter | NTSC                       | PAL                        |
| --------- | -------------------------- | -------------------------- |
| H Total   | 341 dot clocks             | 341 dot clocks             |
| V Total   | 262 lines (263 interlaced) | 312 lines (313 interlaced) |
| Active H  | 256 pixels                 | 256 pixels                 |
| Active V  | 224 lines (239 overscan)   | 224 lines (239 overscan)   |


### Dejitter Note

NTSC SNES outputs a shorter scanline in the non-visible area, which causes sync issues with analog capture devices and scalers such as OSSC. The dejitter mod pauses the pixel clock briefly to normalize scanline length.

**SuperPico:** This is not applicable. SuperPico captures the SNES signal digitally and outputs HDMI directly at 640×480@60Hz from the RP2350B. The HDMI output has its own independent, stable timing — the SNES clock irregularity is absorbed in the capture pipeline and does not propagate to the output. No dejitter hardware is needed.

For other digital mod approaches that output analog RGB to a scaler, a dejitter mod is typically required — see [REFERENCES.md](REFERENCES.md) for prior art.

---

## Known Issues and Solutions

### Issue: Digital output missing brightness control

**Symptom:** Games using brightness fades (SMW level transitions, Super Metroid) appear at full brightness  
**Solution (SuperPico):** Brightness latch on QSB captures D[3:0] on every `$2100` write. RP2350 reads `Brightness[3:0]` once per scanline at HBLANK and applies `(color * brightness) / 15`. See Brightness Control section and Appendix B.

### Issue: Mode 7 shows garbage at screen edges

**Symptom:** Artifacts outside Mode 7 playfield (e.g., Super Mario Kart, F-Zero)  
**Solution (SuperPico):** QSB ANDs `/OVER` (PPU1 p94) and `/TRANSPARENT` (PPU2 p4) into `PIXEL_VALID`. RP2350 outputs color 0 whenever `PIXEL_VALID` is LOW.

### Issue: Super Mario World title screen corrupted

**Symptom:** Star animation shows incorrect colors  
**Solution (SuperPico):** Handled by the same `PIXEL_VALID` signal (see above). `/TRANSPARENT` going LOW during the affected periods suppresses the invalid pixel data.

### Issue: OSSC/scaler compatibility problems

**Symptom:** Picture jitter or sync loss on analog scalers  
**Note:** Not applicable to SuperPico. SuperPico outputs HDMI directly from the RP2350B at 640×480@60Hz with independent stable timing. SNES clock irregularities are absorbed in the capture pipeline and do not affect the HDMI output.

### Issue: Audio dropouts

**Symptom:** Occasional pops or gaps in audio  
**Solution (SuperPico):** Audio is captured via PIO/DMA from S-DSP and embedded in HDMI. Keep SDATA/LRCK/BCLK traces short. The SRC (sample rate converter) in firmware handles the ~32.040kHz → 48kHz conversion with drift compensation. A bad solder joint on LRCK (S-DSP pin 43) is a common source of framing errors — verify with a scope or logic analyzer.

---

## References

See [REFERENCES.md](REFERENCES.md) for all hardware documentation sources, prior art, and credits.

---

## Appendix A: Quick Pin Reference Card

### PPU2 Pins — Core Digital RGB Capture


| Signal             | Pin(s)             | Notes                           |
| ------------------ | ------------------ | ------------------------------- |
| Digital RGB Enable | 93                 | Connect to PPU1 /OVER (pin 94) via QSB logic |
| Red[4:0]           | 81, 80, 79, 78, 77 | TST4–TST0                       |
| Green[4:0]         | 87, 86, 85, 84, 82 | TST9–TST5                       |
| Blue[4:0]          | 92, 91, 90, 89, 88 | TST14–TST10                     |
| HBLANK             | 25                 |                                 |
| VBLANK             | 26                 |                                 |
| /PIXEL CLK         | 27                 |                                 |
| /CSYNC             | 100                |                                 |
| /OVER1             | 37                 | Mode 7 overflow input from PPU1 |
| /OVER2             | 50                 | Mode 7 overflow input from PPU1 |


### PPU2 Pins — Transparency & Pixel Blanking (Mode 7 + SMW fix)


| Signal                | Pin | Notes                                             |
| --------------------- | --- | ------------------------------------------------- |
| /TRANSPARENT (TOUMEI) | 4   | High = opaque pixel; low = transparent/sync/burst |


**Required for:** Mode 7 wrap-around transparency, Super Mario World star animation fix, correct blanking during sync periods.

### PPU2 Pins — Brightness & Fade ($2100 INIDISP)


| Signal  | Pin(s) | Notes                                                                                             |
| ------- | ------ | ------------------------------------------------------------------------------------------------- |
| /PWR    | 6      | CPU write strobe, active low                                                                      |
| D0      | 15     | INIDISP bit 0 — brightness LSB                                                                    |
| D1      | 14     | INIDISP bit 1                                                                                     |
| D2      | 13     | INIDISP bit 2                                                                                     |
| D3      | 12     | INIDISP bit 3 — brightness MSB                                                                    |
| D7      | 8      | INIDISP bit 7 — force-blank; screen forced black *(optional — rarely used mid-frame in practice)* |
| PA0–PA7 | 17–24  | B-bus address; feed into 74HC4078 NOR → ADDR_MATCH signal                                         |


**External ICs required (on QSB):** 74HC4078 (8-input NOR gate) wired to PA0–PA7 produces `ADDR_MATCH` (HIGH when address == `$00`). An AND gate combines `ADDR_MATCH` + `/PWR` into `WRITE_2100`. A 74HC175 (4-bit latch) clocked by `WRITE_2100` holds D[3:0] stable on its outputs. The RP2350 reads `Brightness[3:0]` at any time — no timing-critical bus snooping on the main board.

**Required for:** Any game using fade-to/from-black via $2100 — Super Mario World level transitions, Super Metroid fades, most title screens, etc.

### Essential PPU1 Pins


| Signal         | Pin | Notes                                                |
| -------------- | --- | ---------------------------------------------------- |
| /OVER          | 94  | Mode 7 overflow source; connect to PPU2 pins 37 & 50 |
| PALMODE        | 24  | Region switching — must match PPU2 pin 30            |
| /PIXEL CLK OUT | 93  |                                                      |


### Essential S-DSP Pins for Digital Audio


| Signal | Pin | Notes                 |
| ------ | --- | --------------------- |
| SDATA  | 44  | Serial audio data     |
| LRCK   | 43  | Left/right word clock |
| BCLK   | 42  | Bit clock             |


---

## Appendix B: Daughter Board / QSB — FPC Signal List

This appendix documents the signals that travel from the **daughter board** (soldered near PPU2/PPU1) to the **main RP2350 board** over the FPC connector.

The daughter board preprocesses raw SNES signals before they reach the Pico, reducing FPC pin count and simplifying firmware.

### Signal Flow Diagram

```
PPU2 / PPU1                  QSB Logic                       FPC → RP2350
───────────                  ─────────                       ────────────

TST0–14  ──[routing reorder]──────────────────────────────── RGB[14:0] ┐
HBLANK   ──────────────────────────────────────────────────── HBLANK   │ 18-bit
VBLANK   ──────────────────────────────────────────────────── VBLANK   │ capture
/PCLK    ──────────────────────────────────────────────────── /PCLK    ┘ bus

/TRANSPARENT (PPU2 p4)  ──────────┐
                                  ├──[AND]────────────────── PIXEL_VALID
/OVER        (PPU1 p94) ──────────┘

PA0–PA7  (PPU2 p17–24) ──[8-input NOR]──ADDR_MATCH───┐
                                                     ├──[AND]──WRITE_2100
/PWR     (PPU2 p6)  ─────────────────────────────────┘            │
                                                                  │ (clock)
D0–D3    (PPU2 p12–15) ───────────────────────────────[74HC175]───┴────── Brightness[3:0]

PALMODE  (PPU2 p30)  ──────────────────────────────────────────── PALMODE (optional)
```

### Preprocessing Done on the Daughter Board


| Operation              | Input                           | Output                | IC Required             |
| ---------------------- | ------------------------------- | --------------------- | ----------------------- |
| Address match          | PA0–PA7 (PPU2 pins 17–24)       | `ADDR_MATCH`          | 74HC4078 or NOR tree    |
| Write strobe combine   | `ADDR_MATCH` + `/PWR`           | `WRITE_2100` clock    | AND gate                |
| Brightness latch       | `WRITE_2100` clock + D0–D3      | 4 stable output lines | 74HC175 (4-bit latch)   |
| Pixel validity combine | `/OVER` + `/TRANSPARENT`        | `PIXEL_VALID`         | AND gate                |
| RGB bit reordering     | TST[4:0] per channel (reversed) | Correctly ordered RGB | PCB routing only (free) |


### FPC Signal List

The 18-signal capture bus must map to **contiguous GPIOs** on the RP2350 so the PIO can read the full word in a single instruction.

#### Capture Bus (18 signals — must be contiguous GPIOs)


| Bit | Signal   | Notes                                   |
| --- | -------- | --------------------------------------- |
| 17  | HBLANK   | PPU2 pin 25                             |
| 16  | Red[4]   | TST4 (MSB), reordered on daughter board |
| 15  | Red[3]   | TST3                                    |
| 14  | Red[2]   | TST2                                    |
| 13  | Red[1]   | TST1                                    |
| 12  | Red[0]   | TST0 (LSB), reordered on daughter board |
| 11  | Green[4] | TST9 (MSB), reordered                   |
| 10  | Green[3] | TST8                                    |
| 9   | Green[2] | TST7                                    |
| 8   | Green[1] | TST6                                    |
| 7   | Green[0] | TST5 (LSB), reordered                   |
| 6   | Blue[4]  | TST14 (MSB), reordered                  |
| 5   | Blue[3]  | TST13                                   |
| 4   | Blue[2]  | TST12                                   |
| 3   | Blue[1]  | TST11                                   |
| 2   | Blue[0]  | TST10 (LSB), reordered                  |
| 1   | /PCLK    | PPU2 pin 27                             |
| 0   | VBLANK   | PPU2 pin 26                             |


#### Additional Signals (5–6 signals)


| Signal               | Notes                                                                                                                                              |
| -------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| PIXEL_VALID          | HIGH = pixel valid; LOW = transparent/Mode 7 overflow/sync. Combined from `/OVER` (PPU1 pin 94) AND `/TRANSPARENT` (PPU2 pin 4) on daughter board. |
| Brightness[0]        | D0 latched (INIDISP bit 0)                                                                                                                         |
| Brightness[1]        | D1 latched (INIDISP bit 1)                                                                                                                         |
| Brightness[2]        | D2 latched (INIDISP bit 2)                                                                                                                         |
| Brightness[3]        | D3 latched (INIDISP bit 3, MSB)                                                                                                                    |
| PALMODE *(optional)* | PPU2 pin 30. Pass-through. Add if PAL support is required; otherwise detectable from VBLANK timing.                                                |


### FPC Pin Count Summary


| Group                | Signals |
| -------------------- | ------- |
| Capture bus          | 18      |
| PIXEL_VALID          | 1       |
| Brightness (latched) | 4       |
| **Minimum total**    | **23**  |
| PALMODE (optional)   | +1      |
| **Full total**       | **24**  |


### Audio (Optional — route via same FPC)

If the daughter board is physically near the S-DSP, the audio signals can share the same FPC rather than running separate wires to the main board.


| Signal | S-DSP Pin | Notes                 |
| ------ | --------- | --------------------- |
| SDATA  | 44        | Serial audio data     |
| LRCK   | 43        | Left/right word clock |
| BCLK   | 42        | Bit clock             |


Adding audio brings the FPC to **26–27 pins** total.

---

*Contributions welcome.*