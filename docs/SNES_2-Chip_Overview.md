# SNES 2-Chip Model Architecture Overview

This document provides a high-level architectural overview of the "2-chip" Super Nintendo Entertainment System (SNES/Super Famicom). The term "2-chip" refers to the presence of two separate Picture Processing Units (PPU1 and PPU2), a characteristic of early to mid-lifecycle console revisions.

## System Architecture

The SNES is built around a multi-bus architecture centered on the S-CPU. It utilizes two primary address buses:

1.  **Address Bus A (24-bit):** The general-purpose bus used for accessing ROM (Cartridge), WRAM, and Expansion hardware.
2.  **Address Bus B (8-bit):** A secondary bus dedicated to internal registers, including PPU control, Audio (APU), and DMA.

### Block Diagram (High-Level)

```text
       +-------------------------------------------------------+
       |                     CARTRIDGE SLOT                    |
       +---------------------------+---------------------------+
                                   |
                     Address Bus A (24-bit) / Data Bus (8-bit)
                                   |
       +---------------------------+---------------------------+
       |          S-CPU (5A22)     |          WRAM (128KB)     |
       +-------------+-------------+-------------+-------------+
                     |                           |
               Address Bus B (8-bit)             |
                     |                           |
       +-------------+-------------+             |
       |      S-PPU1 (5C77)        |             |
       +-------------+-------------+             |
                     |                           |
       +-------------+-------------+             |
       |      S-PPU2 (5C78)        |      +------+------+
       +-------------+-------------+      |  S-SMP (APU)|
                     |                    +------+------+
       +-------------+-------------+             |
       |      VIDEO ENCODER        |      +------+------+
       +-------------+-------------+      |  S-DSP (APU)|
                     |                    +------+------+
       +-------------+-------------+             |
       |      MULTI-OUT (A/V)      |      +------+------+
       +---------------------------+      |  AUDIO MIXER|
                                          +-------------+
```

## Primary Integrated Circuits (ICs)

| Component | Part Number | Description | Package |
| :--- | :--- | :--- | :--- |
| **S-CPU** | Ricoh 5A22 | Main CPU (65C816 core) + DMA + I/O | QFP-100 |
| **S-PPU1** | Ricoh 5C77 | Rendering, Backgrounds, Objects, Mode 7 | QFP-100 |
| **S-PPU2** | Ricoh 5C78 | Palettes, Windowing, Mixing, Video DAC | QFP-100 |
| **S-SMP** | Sony SPC700 | Sound Micro-Processor (Audio CPU) | QFP-80 |
| **S-DSP** | Sony S-DSP | Sound Digital Signal Processor | QFP-80 |
| **WRAM** | Various | 128KB Work RAM (Work RAM) | SOP-28/32 |
| **VRAM** | Various | 64KB Video RAM (2x 32KB) | SOP-28 |
| **S-WRAM** | (Optional) | Integrated WRAM controller/chip | QFP-64 |
| **MAD-1** | Nintendo | Memory Address Decoder (Cartridge/System) | SOP-16 |
| **CIC** | F411/D411 | Checking Integrated Circuit (Region Lock) | DIP-16/SOP-16 |

## Board Revisions (2-Chip)

Common "2-chip" motherboard revisions include:
- **SHVC-CPU-01**: The original launch model. Separate APU module on a daughterboard.
- **SNS-CPU-GPM-01/02**: Integrated APU onto the mainboard.
- **SNS-CPU-RGB-01/02**: Improved video output circuitry.
- **SNS-CPU-APU-01**: Final 2-chip revision with most components integrated but still keeping PPU1/2 separate.

## Clock Generation

The system relies on a Master Crystal Oscillator:
- **NTSC**: 21.47727 MHz
- **PAL**: 21.28137 MHz

This clock is divided internally by the CPU and PPUs to generate:
- **CPU Clock**: Variable (1.79 MHz, 2.68 MHz, or 3.58 MHz)
- **Dot Clock**: ~5.37 MHz (NTSC)
- **Audio Clock**: 24.576 MHz (Dedicated crystal for APU)

## Signal Logic Levels
The SNES operates primarily at **5V TTL** logic levels. All main ICs are powered by a +5V rail, typically regulated by a 7805 linear regulator from a 9V-10V DC input.

## Reliable Sources and Technical References

This documentation was reconstructed using the following high-reliability sources:
1. **SNESdev Wiki (nesdev.org):** The primary technical reference for SNES hardware registers, CPU timing, and PPU behavior.
2. **Super Famicom Development Wiki (superfamicom.org):** Extensive documentation on memory mapping, port pinouts, and system-level schematics.
3. **Jonathon Donaldson (j_montes) Schematics:** Highly detailed reverse-engineered schematics of the SHVC-CPU-01 and GPM revisions.
4. **Nintendo Service Manuals:** Official technical documentation for authorized service centers (leaked), providing exact component values and board layouts.
5. **Console5 Wiki:** A comprehensive database of hardware revisions, common failures, and discrete component lists (capacitors/resistors).
6. **Kevtris (Kevin Horton):** Technical notes from one of the most respected figures in console reverse engineering.
