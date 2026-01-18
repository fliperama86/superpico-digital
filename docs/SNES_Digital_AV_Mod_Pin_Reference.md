# SNES Digital AV Mod - Complete Pin Reference

> **Target Repository:** Fork of [michael-hirschmugl/SNES_TST](https://github.com/michael-hirschmugl/SNES_TST)  
> **Original Project:** [Opatusos/SNES_TST](https://github.com/Opatusos/SNES_TST)  
> **Applicable Hardware:** 2-chip/3-chip SNES (NOT 1-CHIP)  
> **Last Updated:** January 2026

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
9. [References](#references)

---

## Overview

This document provides a complete reference for all pins relevant to implementing a digital AV mod for 2-chip/3-chip SNES consoles. The mod extracts native 15-bit digital RGB from the S-PPU2's test pins, bypassing the console's analog video path which introduces blur on non-1CHIP models.

### Why 2-Chip/3-Chip Only?

The 1-CHIP SNES (S-CPUN) consolidates PPU1 and PPU2 into a single die. While it has TST pins, it does **not expose the blanking signals** (OVER, TOUMEI) needed to properly handle Mode 7 transparency and other edge cases. Only the discrete 2-chip PPU2 exposes these critical control signals.

### Project Status (SNES_TST)

The michael-hirschmugl fork contains 109 commits with the following verified/planned features:

| Feature | Status |
|---------|--------|
| Dual analog output | Verified |
| Mode-7 patch (TST pins) | Verified |
| SuperCIC | Verified |
| Dual-frequency oscillator | Not tested |
| Controller support | Partial |
| Dejitter | Not tested |
| D4 region patch | Not tested |
| RGB LED PWM | Not tested |

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

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **93** | **DIGITAL VIDEO ENABLE** | Input | Master enable for digital RGB output. Connect to /OVER for proper blanking. Factory-connected to GND. |
| 77 | TST0 | I/O → Output | Red bit 0 (LSB) |
| 78 | TST1 | I/O → Output | Red bit 1 |
| 79 | TST2 | I/O → Output | Red bit 2 |
| 80 | TST3 | I/O → Output | Red bit 3 |
| 81 | TST4 | I/O → Output | Red bit 4 (MSB) |
| 82 | TST5 | I/O → Output | Green bit 0 (LSB) |
| 84 | TST6 | I/O → Output | Green bit 1 |
| 85 | TST7 | I/O → Output | Green bit 2 |
| 86 | TST8 | I/O → Output | Green bit 3 |
| 87 | TST9 | I/O → Output | Green bit 4 (MSB) |
| 88 | TST10 | I/O → Output | Blue bit 0 (LSB) |
| 89 | TST11 | I/O → Output | Blue bit 1 |
| 90 | TST12 | I/O → Output | Blue bit 2 |
| 91 | TST13 | I/O → Output | Blue bit 3 |
| 92 | TST14 | I/O → Output | Blue bit 4 (MSB) |

**Note:** TST14-12 control test mode selection when DIGITAL VIDEO ENABLE is LOW. Factory-connected to GND.

### Synchronization and Timing Signals

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **100** | **/CSYNC** | Output | Composite sync output. Active low. |
| **1** | **/BURST** | Output | Color burst gate signal. Can be used as alternative sync reference. |
| **27** | **/PIXEL CLK OUT** | Output | 5.37 MHz dot clock output. Goes to expansion port pin 22. |
| **38** | **/PIXEL CLK IN** | Input | 5.37 MHz dot clock input from S-PPU1. |
| **31** | **SYSTEM CLK** | Input | 21.47727 MHz master clock (NTSC). |
| **3** | **COLORBURST CLK** | Input | 3.58 MHz color subcarrier clock. |

### Blanking and Control Signals

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **25** | **HBLANK** | Output | Horizontal blanking signal. |
| **26** | **VBLANK** | Output | Vertical blanking signal. |
| **4** | **/TRANSPARENT (TOUMEI)** | Bidirectional | High when opaque pixel drawn; low during sync/burst. **Critical for Mode 7 fix.** |
| **37** | **/OVER1** | Input | Overflow/wrap signal from S-PPU1. Connect to pin 94 of PPU1. |
| **50** | **/OVER2** | Input | Second overflow input from S-PPU1. Connect to pin 94 of PPU1. |
| **36** | **FIELD** | Input | Interlace field indicator from PPU1. |
| **39** | **/HCLD** | Input | Horizontal counter load from PPU1. |
| **40** | **/VCLD** | Input | Vertical counter load from PPU1. |

### Analog Video Output (For Reference/Hybrid Mods)

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 95 | R | Output | Analog red output (0-700mV typical) |
| 96 | G | Output | Analog green output (0-700mV typical) |
| 97 | B | Output | Analog blue output (0-700mV typical) |
| 94 | +5VA | Power | Analog power supply for video DACs |

### PPU1-PPU2 Communication (Pixel Data from PPU1)

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 41 | COLOR0 | Input | Color index bit 0 from PPU1 |
| 42 | COLOR1 | Input | Color index bit 1 from PPU1 |
| 43 | COLOR2 | Input | Color index bit 2 from PPU1 |
| 44 | PRIO0 | Input | Priority bit 0 from PPU1 |
| 45 | PRIO1 | Input | Priority bit 1 from PPU1 |
| 46-49 | CHR0-CHR3 | Input | Character data from PPU1 |

### External Video Input (EXT Pins)

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 69-76 | EXT0-EXT7 | Input | External video input, connected to VDB7..0. Used for external video mixing. |

### Reset and Mode Configuration

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 28 | /RESOUT1 | Output | Main reset output to CPU, APU, cartridge, expansion port. |
| 33 | /RESOUT0 | Output | Reset output to S-PPU1. |
| 34 | /RESET | Input | Reset input from CIC. |
| 29 | /EXTLATCH | Input | H/V counter latch control. Connected to joypad IO D7. |
| 30 | PALMODE | Input | PAL/NTSC mode select. High = PAL (50Hz), Low = NTSC (60Hz). |
| 98 | HVCMODE | Input | H/V counter mode configuration. |

### CPU Interface

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 8-15 | CPU D7-D0 | Bidirectional | 8-bit CPU data bus |
| 17-24 | PA7-PA0 | Input | 8-bit PPU address bus (B-bus) |
| 6 | /PWR | Input | CPU write strobe |
| 7 | /PRD | Input | CPU read strobe |

### VRAM Data Interface

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| 51-58 | VDB0-VDB7 | Input | VRAM data bus B |
| 60-67 | VDA0-VDA7 | Input | VRAM data bus A |

---

## S-PPU1 (5C77) - Control Signals Source

The S-PPU1 (Nintendo 5C77) is also a QFP-100 package with 0.65mm pitch. It handles sprite/background rendering, VRAM addressing, and generates control signals needed by PPU2.

### Critical Control Signals for Digital Video Mod

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **94** | **/OVER** | Output | **Mode 7 overflow/wrap indicator.** Connect to PPU2 pins 37 and 50, and to DIGITAL VIDEO ENABLE (pin 93) via logic. When low, indicates pixel is outside the Mode 7 playfield. |
| 93 | /PIXEL CLK OUT | Output | 5.37 MHz pixel clock output to PPU2. |
| 95 | FIELD | Bidirectional | Interlace field signal, shared with PPU2. |
| 92 | /HCLD | Bidirectional | Horizontal counter load. |
| 91 | /VCLD | Bidirectional | Vertical counter load. |

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

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **24** | **PALMODE** | Input | PAL/NTSC mode. Must match PPU2 pin 30 for region switching. |
| 23 | HVCMODE | Input | H/V counter mode. |
| 25 | /MASTER | Input | Master/slave mode. Normally grounded. |
| 26 | /EXTSYNC | Input | External sync input. |

---

## S-DSP - Digital Audio Source

The S-DSP outputs serial digital audio that can be converted to S/PDIF for pristine audio extraction. The audio format is 32kHz, 16-bit stereo.

### S-DSP Digital Audio Pins (QFP-80, 0.8mm pitch)

| Pin | Name | Direction | Function |
|-----|------|-----------|----------|
| **44** | **DAC DATA (SDATA)** | Output | Serial audio data. 16-bit samples, MSB first. |
| **43** | **DAC LEFT/RIGHT CLK (LRCK)** | Output | Word select / Left-Right clock. ~32kHz (actually ~32.040kHz). |
| **42** | **DAC BIT CLK (BCLK)** | Output | Bit clock for serial data. |
| 46 | CRYSTAL IN | Input | 24.576MHz crystal input. |
| 45 | CRYSTAL OUT | Output | Crystal oscillator output. |
| 47 | /RESET | Input | Reset input. |
| 39 | /MUTE | Output | Mute control output to DAC. |

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

For S/PDIF conversion, use a chip like the Cirrus Logic CS8405A or CS8406, or Texas Instruments DIT4096. Connect:

| S-DSP Pin | S/PDIF Encoder Pin | Notes |
|-----------|-------------------|-------|
| Pin 44 (SDATA) | SDIN | Serial data input |
| Pin 43 (LRCK) | LRCK/FSYNC | Word clock / Frame sync |
| Pin 42 (BCLK) | SCLK | Serial clock |

**Important:** The S-DSP outputs at approximately 32.040kHz (not exactly 32kHz) due to the 24.576MHz crystal. Replace the ceramic resonator X2 with a precision crystal oscillator for stable output.

---

## CIC/SuperCIC - Region and Reset Control

The CIC (Checking Integrated Circuit) handles region lockout. For a complete AV mod with region-free capability, SuperCIC replacement is recommended.

### Original CIC Lock Chip (18-pin, in console)

| Pin | Name | Function |
|-----|------|----------|
| 1 | D2 | Data line 2 to cartridge CIC |
| 2 | D1 | Data line 1 to cartridge CIC |
| 3 | SEED | Seed input |
| 4 | ?? | Mode select (lift for region bypass on F41x chips) |
| 7 | CLK | Clock input from S-CLK or cart slot pin 56 |
| 8 | RST | Reset input |
| 10 | /RESET (Cart) | Reset output to cartridge |
| 11 | /RESET (Console) | Reset output to console |

### SuperCIC Integration Points

| Signal | PPU Connection | Function |
|--------|---------------|----------|
| PALMODE output | PPU1 pin 24, PPU2 pin 30 | Sets video timing (50Hz/60Hz) |
| D4 patch output | PPU2 CPU D4 (pin 11) | Region patch for $213F reads |
| Clock input | CIC pad 7 | SuperCIC clock source |
| Reset output | System reset line | Directly replaces CIC reset |

### D4 Region Patch

The D4 patch overrides bit 4 of PPU2's $213F status register response, which reports PAL/NTSC mode to the CPU. This is necessary because some games check this register for region verification.

---

## Implementation Notes

### Brightness Control Problem

The TST pins output raw 5-bit-per-channel RGB **before** the brightness multiplier is applied. The SNES applies brightness via register $2100 (INIDISP), which controls a 4-bit DAC used as reference voltage for the analog RGB DACs.

**Solution (Opatus SNES_TST approach):**
- Snoop CPU writes to $2100 on the address/data bus
- Implement 3× 5×4-bit multipliers in CPLD/FPGA
- Scale digital RGB values accordingly
- Output via external DAC (e.g., ADV7123) for analog, or directly for HDMI

### Mode 7 Transparency Fix

When Mode 7 wraps around (pixels outside the 128×128 tile map with transparency mode enabled), /OVER from PPU1 goes low. The digital output continues showing stale data instead of transparency.

**Solution:**
1. Connect PPU1 pin 94 (/OVER) to your CPLD
2. Pull PPU2 /OVER1 (pin 37) and /OVER2 (pin 50) HIGH via the CPLD when transparency is needed
3. Use /TRANSPARENT (TOUMEI, PPU2 pin 4) to gate valid pixel data
4. Output color 0 (transparent) when /OVER is low and Mode 7 wrap mode 2 is active

### TOUMEI Signal Usage

The /TRANSPARENT signal (PPU2 pin 4) is HIGH during valid picture output and LOW during sync/burst periods. Use this to:
- Gate the digital video output
- Fix Super Mario World title screen issues
- Properly blank during horizontal/vertical sync

### Recommended CPLD Connections Summary

| CPLD Input | Source | Purpose |
|------------|--------|---------|
| TST0-TST14 | PPU2 pins 77-92 | 15-bit digital RGB |
| /OVER | PPU1 pin 94 | Mode 7 wrap detection |
| /TRANSPARENT | PPU2 pin 4 | Valid pixel indicator |
| /CSYNC | PPU2 pin 100 | Composite sync |
| HBLANK | PPU2 pin 25 | Horizontal blank |
| VBLANK | PPU2 pin 26 | Vertical blank |
| /PIXEL CLK | PPU2 pin 27 | Dot clock (5.37MHz) |
| SYSTEM CLK | PPU2 pin 31 | Master clock (21.47MHz) |
| CPU D0-D7 | PPU2 pins 8-15 | Brightness snoop |
| CPU PA0-PA7 | PPU2 pins 17-24 | Address snoop for $2100 |
| /PWR | PPU2 pin 6 | CPU write strobe |

| CPLD Output | Destination | Purpose |
|-------------|-------------|---------|
| DIGITAL VIDEO ENABLE | PPU2 pin 93 | Enable TST output |
| /OVER1 override | PPU2 pin 37 | Mode 7 fix |
| /OVER2 override | PPU2 pin 50 | Mode 7 fix |
| RGB out (post-brightness) | DAC or HDMI encoder | Final video output |

---

## Signal Timing and Clocks

### Clock Frequencies

| Clock | NTSC | PAL | Source |
|-------|------|-----|--------|
| Master Clock | 21.47727 MHz | 21.28137 MHz | Crystal oscillator |
| Dot Clock | 5.3693175 MHz | 5.3203425 MHz | Master ÷ 4 |
| Color Burst | 3.579545 MHz | 4.43361875 MHz | Subcarrier |
| Audio Sample Rate | ~32.040 kHz | ~32.040 kHz | S-DSP crystal |

### Video Timing

| Parameter | NTSC | PAL |
|-----------|------|-----|
| H Total | 341 dot clocks | 341 dot clocks |
| V Total | 262 lines (263 interlaced) | 312 lines (313 interlaced) |
| Active H | 256 pixels | 256 pixels |
| Active V | 224 lines (239 overscan) | 224 lines (239 overscan) |

### Dejitter Note

NTSC SNES outputs a shorter scanline in the non-visible area, causing sync issues with some capture devices and scalers (notably OSSC in certain modes). The dejitter mod pauses the clock briefly to normalize scanline length. This is implemented in the SNES_TST CPLD firmware (currently listed as "not tested").

---

## Known Issues and Solutions

### Issue: Digital output missing brightness control
**Symptom:** Games using brightness fades appear at full brightness  
**Solution:** Implement $2100 register snooping and 5×4-bit multipliers in CPLD

### Issue: Mode 7 shows garbage at screen edges
**Symptom:** Artifacts outside Mode 7 playfield (e.g., Super Mario Kart, F-Zero)  
**Solution:** Monitor /OVER signal, force transparency when appropriate

### Issue: Super Mario World title screen corrupted
**Symptom:** Star animation shows incorrect colors  
**Solution:** Use TOUMEI signal to properly gate output

### Issue: Ship duplication in Super Metroid intro
**Symptom:** Visible when switching between analog and digital RGB  
**Solution:** PPU pipeline delay causes desync; avoid mid-frame switching

### Issue: OSSC/scaler compatibility problems
**Symptom:** Picture jitter or sync loss  
**Solution:** Implement dejitter in CPLD; use stable clock generation

### Issue: Audio dropouts with S/PDIF mod
**Symptom:** Occasional pops or gaps in digital audio  
**Solution:** Keep serial data wires short; replace ceramic resonator with crystal oscillator

---

## References

### Primary Sources

- **SNESdev Wiki PPU Pinout:** https://snes.nesdev.org/wiki/PPU_pinout
- **SNESdev Wiki APU Pinout:** https://snes.nesdev.org/wiki/APU_pinout
- **Fullsnes (nocash):** https://problemkaputt.de/fullsnes.htm
- **Super Famicom Development Wiki:** https://wiki.superfamicom.org/

### Project Repositories

- **SNES_TST (michael-hirschmugl fork):** https://github.com/michael-hirschmugl/SNES_TST
- **SNES_TST (Opatusos original):** https://github.com/Opatusos/SNES_TST
- **SNES MultiRegion with DeJitter:** https://github.com/borti4938/SNES_MultiRegion_with_DeJitter_QID

### Forum Discussions

- **Shmups.system11.org TST Thread:** https://shmups.system11.org/viewtopic.php?f=6&t=66597
- **Circuit-board.de Decapped PPU Analysis:** https://circuit-board.de/forum/index.php/Thread/25396

### Related Mods

- **SNES/PDIF Digital Audio (qwertymodo):** https://www.qwertymodo.com/hardware-projects/snes/snespdif-digital-audio-mod
- **SuperCIC Project:** https://sd2snes.de/blog/cool-stuff/supercic

### Schematics

- **jwdonal Official SNES Schematics:** Available on gamesx.com wiki
- **1-CHIP PAL SNES Schematic:** https://videogameperfection.com/forums/topic/schematic-for-1chip-pal-snes/

---

## Appendix A: Quick Pin Reference Card

### Essential PPU2 Pins for Digital Video

```
Digital RGB Enable:  Pin 93 (connect via CPLD to /OVER logic)
Red[4:0]:           Pins 81,80,79,78,77 (TST4-TST0)
Green[4:0]:         Pins 87,86,85,84,82 (TST9-TST5)
Blue[4:0]:          Pins 92,91,90,89,88 (TST14-TST10)
/CSYNC:             Pin 100
/TRANSPARENT:       Pin 4
HBLANK:             Pin 25
VBLANK:             Pin 26
/PIXEL CLK:         Pin 27
SYSTEM CLK:         Pin 31
/OVER1:             Pin 37 (from PPU1)
/OVER2:             Pin 50 (from PPU1)
```

### Essential PPU1 Pins

```
/OVER:              Pin 94 (critical for Mode 7 fix)
PALMODE:            Pin 24 (region switching)
/PIXEL CLK OUT:     Pin 93
```

### Essential S-DSP Pins for Digital Audio

```
SDATA:              Pin 44
LRCK:               Pin 43
BCLK:               Pin 42
```

---

*Document maintained for the neopico-hd and SNES_TST communities. Contributions welcome.*
