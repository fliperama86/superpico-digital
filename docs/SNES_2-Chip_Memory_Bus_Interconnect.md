# SNES 2-Chip Memory and Bus Interconnect (Netlist Level)

This document describes the critical interconnections between the major ICs in the 2-chip SNES. This is as close to a "netlist" as possible without providing a full PCB layout.

## 1. CPU <-> PPU Interconnect (Bus B)

The CPU (5A22) communicates with PPU1 and PPU2 primarily over Address Bus B.

| CPU Pin | PPU1 Pin | PPU2 Pin | Signal |
| :--- | :--- | :--- | :--- |
| 52-59 (PA0-7) | 58-65 (PA0-7) | 10-17 (PA0-7) | Address Bus B (8-bit) |
| 82-89 (D0-7) | 49-56 (D0-7) | 1-8 (D0-7) | Data Bus (8-bit) |
| 60 (/PARD) | 67 (/PARD) | 19 (/PARD) | Bus B Read Enable |
| 61 (/PAWR) | 68 (/PAWR) | 20 (/PAWR) | Bus B Write Enable |
| 77 (/PPU1_CS) | 69 (/PPU1CS) | - | PPU1 Select |
| 78 (/PPU2_CS) | - | 21 (/PPU2CS) | PPU2 Select |

## 2. PPU1 <-> PPU2 Internal Video Bus

These signals are direct chip-to-chip connections for video synchronization and pixel mixing.

| PPU1 Pin | PPU2 Pin | Signal | Description |
| :--- | :--- | :--- | :--- |
| 72-79 (COLOR0-7)| 22-30 (COLOR0-7)| Pixel Color Index | 8-bit index for palette lookup |
| 81-82 (PRIO0-1) | 31-32 (PRIO0-1) | Pixel Priority | 2-bit priority for mixing |
| 94 (PCLK) | 35 (PCLK) | Pixel Clock | Synchronizes the two PPUs |
| 89 (HSYNC) | 36 (HSYNC) | Horizontal Sync | System-wide horizontal timing |
| 90 (VSYNC) | 37 (VSYNC) | Vertical Sync | System-wide vertical timing |
| 92 (BLANK) | 38 (BLANK) | Blanking | Indicates display blanking period |

## 3. PPU <-> VRAM Interconnect

PPU1 acts as the master for the 64KB VRAM. The VRAM is organized as two 8-bit chips (High and Low) to provide a 16-bit wide bus.

| PPU1 Pin | VRAM Chips | Signal |
| :--- | :--- | :--- |
| 1-15 (VA0-13) | Address Pins 0-13 | VRAM Address Bus |
| 17-24 (VAA0-7) | Chip A (Low) Data 0-7 | VRAM Data Bus A |
| 26-33 (VAB0-7) | Chip B (High) Data 0-7| VRAM Data Bus B |
| 35 (/VAWR) | Chip A /WE | VRAM Write Enable A |
| 36 (/VBWR) | Chip B /WE | VRAM Write Enable B |
| 37 (/VARD) | Chip A /OE | VRAM Read Enable A |
| 38 (/VBRD) | Chip B /OE | VRAM Read Enable B |

## 4. CPU <-> WRAM Interconnect

The 128KB WRAM (Work RAM) is usually accessed via Address Bus A.

| CPU Pin | WRAM Pins | Signal |
| :--- | :--- | :--- |
| 6-22, 24-28 (A0-20)| Address Pins 0-16 | WRAM Address Bus |
| 82-89 (D0-7) | Data Pins 0-7 | Data Bus |
| 36 (/WRAM) | /CS or /OE | WRAM Select (Bus A) |
| 81 (/WRAM_CS) | /CS | WRAM Select (Bus B DMA) |
| 32 (/RD) | /OE | Read Enable |
| 33 (/WR) | /WE | Write Enable |

## 5. System Reset and Timing

| Source | Target | Signal | Description |
| :--- | :--- | :--- | :--- |
| CIC Pin 1 | CPU 38, PPU1 70, PPU2 34| /RESET | Global System Reset |
| PPU2 | CPU 40 | /NMI | Vertical Blank Interrupt |
| PPU2 | CPU 75 | /VBLANK | V-Blank Timing Signal |
| PPU2 | CPU 76 | /HBLANK | H-Blank Timing Signal |
