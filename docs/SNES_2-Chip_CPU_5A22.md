# S-CPU: Ricoh 5A22 (100-pin QFP)

The S-CPU is the heart of the SNES, based on a 16-bit WDC 65C816 core. It includes additional circuitry for DMA, HDMA, hardware math (mult/div), and I/O.

## Pinout Table

| Pin | Name | Type | Description |
| :--- | :--- | :--- | :--- |
| 1 | D4 | I/O | Data Bus Bit 4 |
| 2 | D5 | I/O | Data Bus Bit 5 |
| 3 | D6 | I/O | Data Bus Bit 6 |
| 4 | D7 | I/O | Data Bus Bit 7 |
| 5 | GND | Pwr | Ground |
| 6 | A0 | Out | Address Bus Bit 0 |
| 7 | A1 | Out | Address Bus Bit 1 |
| 8 | A2 | Out | Address Bus Bit 2 |
| 9 | A3 | Out | Address Bus Bit 3 |
| 10 | A4 | Out | Address Bus Bit 4 |
| 11 | A5 | Out | Address Bus Bit 5 |
| 12 | A6 | Out | Address Bus Bit 6 |
| 13 | A7 | Out | Address Bus Bit 7 |
| 14 | VCC | Pwr | +5V Power |
| 15 | A8 | Out | Address Bus Bit 8 |
| 16 | A9 | Out | Address Bus Bit 9 |
| 17 | A10 | Out | Address Bus Bit 10 |
| 18 | A11 | Out | Address Bus Bit 11 |
| 19 | A12 | Out | Address Bus Bit 12 |
| 20 | A13 | Out | Address Bus Bit 13 |
| 21 | A14 | Out | Address Bus Bit 14 |
| 22 | A15 | Out | Address Bus Bit 15 |
| 23 | GND | Pwr | Ground |
| 24 | A16 | Out | Address Bus Bit 16 |
| 25 | A17 | Out | Address Bus Bit 17 |
| 26 | A18 | Out | Address Bus Bit 18 |
| 27 | A19 | Out | Address Bus Bit 19 |
| 28 | A20 | Out | Address Bus Bit 20 |
| 29 | A21 | Out | Address Bus Bit 21 |
| 30 | A22 | Out | Address Bus Bit 22 |
| 31 | A23 | Out | Address Bus Bit 23 |
| 32 | /RD | Out | Memory Read (Active Low) |
| 33 | /WR | Out | Memory Write (Active Low) |
| 34 | VCC | Pwr | +5V Power |
| 35 | VERSION | Out | Version ID (Hardwired) |
| 36 | /WRAM | Out | Work RAM Select (Active Low) |
| 37 | /ROM | Out | Cartridge ROM Select (Active Low) |
| 38 | /CPURST | In | CPU Reset (from CIC/Reset Circuit) |
| 39 | /IRQ | In | Interrupt Request (Active Low) |
| 40 | /NMI | In | Non-Maskable Interrupt (from PPU2) |
| 41 | /VRAM_RD | Out | VRAM Read Control |
| 42 | /VRAM_WR | Out | VRAM Write Control |
| 43 | /CART | In | Cartridge Inserted Sense |
| 44 | VCC | Pwr | +5V Power |
| 45 | NC | - | No Connection |
| 46 | XIN | In | Master Clock Input (21.47 MHz) |
| 47 | XOUT | Out | Master Clock Output |
| 48 | GND | Pwr | Ground |
| 49 | PHIL | Out | Phase L Clock |
| 50 | PHIH | Out | Phase H Clock |
| 51 | /REFRESH | Out | DRAM Refresh Signal |
| 52 | PA0 | Out | Address Bus B Bit 0 |
| 53 | PA1 | Out | Address Bus B Bit 1 |
| 54 | PA2 | Out | Address Bus B Bit 2 |
| 55 | PA3 | Out | Address Bus B Bit 3 |
| 56 | PA4 | Out | Address Bus B Bit 4 |
| 57 | PA5 | Out | Address Bus B Bit 5 |
| 58 | PA6 | Out | Address Bus B Bit 6 |
| 59 | PA7 | Out | Address Bus B Bit 7 |
| 60 | /PARD | Out | Address Bus B Read (Active Low) |
| 61 | /PAWR | Out | Address Bus B Write (Active Low) |
| 62 | VCC | Pwr | +5V Power |
| 63 | /IO | Out | I/O Port Select |
| 64 | CONT1_D0 | In | Controller Port 1 Data 0 |
| 65 | CONT1_D1 | In | Controller Port 1 Data 1 |
| 66 | CONT2_D0 | In | Controller Port 2 Data 0 |
| 67 | CONT2_D1 | In | Controller Port 2 Data 1 |
| 68 | IO_CLK | Out | Controller Shift Clock |
| 69 | IO_LATCH | Out | Controller Latch |
| 70 | GND | Pwr | Ground |
| 71 | OUT0 | Out | I/O Out 0 (Expansion) |
| 72 | OUT1 | Out | I/O Out 1 (Expansion) |
| 73 | OUT2 | Out | I/O Out 2 (Expansion) |
| 74 | /RESOUT | Out | System Reset Output |
| 75 | /VBLANK | In | V-Blank Input (from PPU2) |
| 76 | /HBLANK | In | H-Blank Input (from PPU2) |
| 77 | /PPU1_CS | Out | PPU1 Chip Select |
| 78 | /PPU2_CS | Out | PPU2 Chip Select |
| 79 | VCC | Pwr | +5V Power |
| 80 | /APU_CS | Out | APU Chip Select |
| 81 | /WRAM_CS | Out | WRAM Chip Select |
| 82 | D0 | I/O | Data Bus Bit 0 |
| 83 | D1 | I/O | Data Bus Bit 1 |
| 84 | D2 | I/O | Data Bus Bit 2 |
| 85 | D3 | I/O | Data Bus Bit 3 |
| 86-100 | ... | ... | (Power/Ground and specialized test pins) |

*(Note: Pins 86-100 include additional GND/VCC and specialized signals used during manufacturing or in specific sub-revisions.)*

## Architecture Features

- **8/16-bit Core:** Capable of switching between 8-bit and 16-bit modes.
- **DMA Engine:** 8 independent channels for high-speed data transfer to PPU (Bus B).
- **HDMA:** Horizontal DMA for per-scanline register updates.
- **Hardware Math:** Dedicated registers for 16-bit multiplication and 32-bit/16-bit division.
- **Address Spaces:**
    - **Bank $00-$3F, $80-$BF:** System Area (Registers, RAM, Low-ROM).
    - **Bank $40-$7D, $C0-$FF:** Data Area (Hi-ROM, Extended RAM).
    - **Bank $7E-$7F:** WRAM (128 KB).
