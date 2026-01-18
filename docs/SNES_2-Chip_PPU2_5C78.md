# S-PPU2: Ricoh 5C78 (100-pin QFP)

The S-PPU2 is the backend of the SNES video system. It handles color palette lookup (CGRAM), windowing effects, color math (blending), and the final Digital-to-Analog Conversion (DAC).

## Pinout Table (S-PPU2)

| Pin | Name | Type | Description |
| :--- | :--- | :--- | :--- |
| 1 | D0 | I/O | CPU Data Bus Bit 0 |
| 2 | D1 | I/O | CPU Data Bus Bit 1 |
| 3 | D2 | I/O | CPU Data Bus Bit 2 |
| 4 | D3 | I/O | CPU Data Bus Bit 3 |
| 5 | D4 | I/O | CPU Data Bus Bit 4 |
| 6 | D5 | I/O | CPU Data Bus Bit 5 |
| 7 | D6 | I/O | CPU Data Bus Bit 6 |
| 8 | D7 | I/O | CPU Data Bus Bit 7 |
| 9 | VCC | Pwr | +5V Power |
| 10 | PA0 | In | Address Bus B Bit 0 |
| 11 | PA1 | In | Address Bus B Bit 1 |
| 12 | PA2 | In | Address Bus B Bit 2 |
| 13 | PA3 | In | Address Bus B Bit 3 |
| 14 | PA4 | In | Address Bus B Bit 4 |
| 15 | PA5 | In | Address Bus B Bit 5 |
| 16 | PA6 | In | Address Bus B Bit 6 |
| 17 | PA7 | In | Address Bus B Bit 7 |
| 18 | GND | Pwr | Ground |
| 19 | /PARD | In | PPU Read Enable |
| 20 | /PAWR | In | PPU Write Enable |
| 21 | /PPU2CS | In | PPU2 Chip Select |
| 22 | COLOR0 | In | Color Data Bit 0 (from PPU1) |
| 23 | COLOR1 | In | Color Data Bit 1 (from PPU1) |
| 24 | COLOR2 | In | Color Data Bit 2 (from PPU1) |
| 25 | VCC | Pwr | +5V Power |
| 26 | COLOR3 | In | Color Data Bit 3 (from PPU1) |
| 27 | COLOR4 | In | Color Data Bit 4 (from PPU1) |
| 28 | COLOR5 | In | Color Data Bit 5 (from PPU1) |
| 29 | COLOR6 | In | Color Data Bit 6 (from PPU1) |
| 30 | COLOR7 | In | Color Data Bit 7 (from PPU1) |
| 31 | PRIO0 | In | Priority Bit 0 (from PPU1) |
| 32 | PRIO1 | In | Priority Bit 1 (from PPU1) |
| 33 | GND | Pwr | Ground |
| 34 | /RESET | In | System Reset |
| 35 | PCLK | In | Pixel Clock (from PPU1) |
| 36 | HSYNC | I/O | Horizontal Sync |
| 37 | VSYNC | I/O | Vertical Sync |
| 38 | BLANK | In | Blanking Signal (from PPU1) |
| 39 | VCC | Pwr | +5V Power |
| 40 | EXT0 | Out | External Video Data 0 |
| 41 | EXT1 | Out | External Video Data 1 |
| 42 | EXT2 | Out | External Video Data 2 |
| 43 | EXT3 | Out | External Video Data 3 |
| 44 | EXT4 | Out | External Video Data 4 |
| 45 | EXT5 | Out | External Video Data 5 |
| 46 | EXT6 | Out | External Video Data 6 |
| 47 | EXT7 | Out | External Video Data 7 |
| 48 | GND | Pwr | Ground |
| 49 | R0 | Out | Red DAC Output (Bit 0) |
| 50 | R1 | Out | Red DAC Output (Bit 1) |
| 51 | R2 | Out | Red DAC Output (Bit 2) |
| 52 | R3 | Out | Red DAC Output (Bit 3) |
| 53 | R4 | Out | Red DAC Output (Bit 4) |
| 54 | G0 | Out | Green DAC Output (Bit 0) |
| 55 | G1 | Out | Green DAC Output (Bit 1) |
| 56 | G2 | Out | Green DAC Output (Bit 2) |
| 57 | VCC | Pwr | +5V Power |
| 58 | G3 | Out | Green DAC Output (Bit 3) |
| 59 | G4 | Out | Green DAC Output (Bit 4) |
| 60 | B0 | Out | Blue DAC Output (Bit 0) |
| 61 | B1 | Out | Blue DAC Output (Bit 1) |
| 62 | B2 | Out | Blue DAC Output (Bit 2) |
| 63 | B3 | Out | Blue DAC Output (Bit 3) |
| 64 | B4 | Out | Blue DAC Output (Bit 4) |
| 65 | CSYNC | Out | Composite Sync |
| 66 | GND | Pwr | Ground |
| 67 | VOUT | Out | Composite Video Output (Analog) |
| 68 | Y | Out | Luminance (S-Video) |
| 69 | C | Out | Chrominance (S-Video) |
| 70-100| ... | ... | (Power/Ground and specialized test pins) |

*(Note: In many 2-chip revisions, the R/G/B/Y/C signals are then sent to a Video Encoder chip like the S-ENC or BA6592F for final amplification and multi-out formatting.)*

## Functional Responsibilities

- **CGRAM (Color RAM):** Internal storage for 256 colors (15-bit RGB).
- **Color Math:** Implements addition and subtraction between background and sub-screen layers.
- **Windowing:** Handles rectangular or sprite-based clipping windows.
- **Status Signals:** Generates V-Blank and H-Blank signals used by the CPU for timing.
- **Video DAC:** Converts internal digital color values into analog R, G, B, and Sync signals.
- **Inter-PPU Communication:** Receives 8-bit color index and 2-bit priority from PPU1 to determine the final pixel to display.
