# S-PPU1: Ricoh 5C77 (100-pin QFP)

The S-PPU1 is the primary rendering engine of the SNES. It handles background layers, object (sprite) data fetching, and Mode 7 transformations.

## Pinout Table (S-PPU1)

| Pin | Name | Type | Description |
| :--- | :--- | :--- | :--- |
| 1 | VA0 | Out | VRAM Address Bit 0 |
| 2 | VA1 | Out | VRAM Address Bit 1 |
| 3 | VA2 | Out | VRAM Address Bit 2 |
| 4 | VA3 | Out | VRAM Address Bit 3 |
| 5 | VA4 | Out | VRAM Address Bit 4 |
| 6 | VA5 | Out | VRAM Address Bit 5 |
| 7 | VA6 | Out | VRAM Address Bit 6 |
| 8 | VA7 | Out | VRAM Address Bit 7 |
| 9 | VCC | Pwr | +5V Power |
| 10 | VA8 | Out | VRAM Address Bit 8 |
| 11 | VA9 | Out | VRAM Address Bit 9 |
| 12 | VA10 | Out | VRAM Address Bit 10 |
| 13 | VA11 | Out | VRAM Address Bit 11 |
| 14 | VA12 | Out | VRAM Address Bit 12 |
| 15 | VA13 | Out | VRAM Address Bit 13 |
| 16 | GND | Pwr | Ground |
| 17 | VAA0 | I/O | VRAM Data Bus A Bit 0 |
| 18 | VAA1 | I/O | VRAM Data Bus A Bit 1 |
| 19 | VAA2 | I/O | VRAM Data Bus A Bit 2 |
| 20 | VAA3 | I/O | VRAM Data Bus A Bit 3 |
| 21 | VAA4 | I/O | VRAM Data Bus A Bit 4 |
| 22 | VAA5 | I/O | VRAM Data Bus A Bit 5 |
| 23 | VAA6 | I/O | VRAM Data Bus A Bit 6 |
| 24 | VAA7 | I/O | VRAM Data Bus A Bit 7 |
| 25 | VCC | Pwr | +5V Power |
| 26 | VAB0 | I/O | VRAM Data Bus B Bit 0 |
| 27 | VAB1 | I/O | VRAM Data Bus B Bit 1 |
| 28 | VAB2 | I/O | VRAM Data Bus B Bit 2 |
| 29 | VAB3 | I/O | VRAM Data Bus B Bit 3 |
| 30 | VAB4 | I/O | VRAM Data Bus B Bit 4 |
| 31 | VAB5 | I/O | VRAM Data Bus B Bit 5 |
| 32 | VAB6 | I/O | VRAM Data Bus B Bit 6 |
| 33 | VAB7 | I/O | VRAM Data Bus B Bit 7 |
| 34 | GND | Pwr | Ground |
| 35 | /VAWR | Out | VRAM Write Enable (A) |
| 36 | /VBWR | Out | VRAM Write Enable (B) |
| 37 | /VARD | Out | VRAM Read Enable (A) |
| 38 | /VBRD | Out | VRAM Read Enable (B) |
| 39 | VCC | Pwr | +5V Power |
| 40 | EXT0 | In | External Sync/Data 0 |
| 41 | EXT1 | In | External Sync/Data 1 |
| 42 | EXT2 | In | External Sync/Data 2 |
| 43 | EXT3 | In | External Sync/Data 3 |
| 44 | EXT4 | In | External Sync/Data 4 |
| 45 | EXT5 | In | External Sync/Data 5 |
| 46 | EXT6 | In | External Sync/Data 6 |
| 47 | EXT7 | In | External Sync/Data 7 |
| 48 | GND | Pwr | Ground |
| 49 | D0 | I/O | CPU Data Bus Bit 0 |
| 50 | D1 | I/O | CPU Data Bus Bit 1 |
| 51 | D2 | I/O | CPU Data Bus Bit 2 |
| 52 | D3 | I/O | CPU Data Bus Bit 3 |
| 53 | D4 | I/O | CPU Data Bus Bit 4 |
| 54 | D5 | I/O | CPU Data Bus Bit 5 |
| 55 | D6 | I/O | CPU Data Bus Bit 6 |
| 56 | D7 | I/O | CPU Data Bus Bit 7 |
| 57 | VCC | Pwr | +5V Power |
| 58 | PA0 | In | Address Bus B Bit 0 |
| 59 | PA1 | In | Address Bus B Bit 1 |
| 60 | PA2 | In | Address Bus B Bit 2 |
| 61 | PA3 | In | Address Bus B Bit 3 |
| 62 | PA4 | In | Address Bus B Bit 4 |
| 63 | PA5 | In | Address Bus B Bit 5 |
| 64 | PA6 | In | Address Bus B Bit 6 |
| 65 | PA7 | In | Address Bus B Bit 7 |
| 66 | GND | Pwr | Ground |
| 67 | /PARD | In | PPU Read Enable |
| 68 | /PAWR | In | PPU Write Enable |
| 69 | /PPU1CS | In | PPU1 Chip Select |
| 70 | RESET | In | System Reset |
| 71 | VCC | Pwr | +5V Power |
| 72 | COLOR0 | Out | Color Data Bit 0 (to PPU2) |
| 73 | COLOR1 | Out | Color Data Bit 1 (to PPU2) |
| 74 | COLOR2 | Out | Color Data Bit 2 (to PPU2) |
| 75 | COLOR3 | Out | Color Data Bit 3 (to PPU2) |
| 76 | COLOR4 | Out | Color Data Bit 4 (to PPU2) |
| 77 | COLOR5 | Out | Color Data Bit 5 (to PPU2) |
| 78 | COLOR6 | Out | Color Data Bit 6 (to PPU2) |
| 79 | COLOR7 | Out | Color Data Bit 7 (to PPU2) |
| 80 | GND | Pwr | Ground |
| 81 | PRIO0 | Out | Priority Bit 0 (to PPU2) |
| 82 | PRIO1 | Out | Priority Bit 1 (to PPU2) |
| 83 | MASTER | Out | Master Clock Output |
| 84 | HCLD | In | Horizontal Counter Load |
| 85 | VCLD | In | Vertical Counter Load |
| 86 | VCC | Pwr | +5V Power |
| 87 | FIELD | Out | Interlace Field Indicator |
| 88 | OVER | Out | Sprite Overflow Indicator |
| 89 | HSYNC | I/O | Horizontal Sync |
| 90 | VSYNC | I/O | Vertical Sync |
| 91 | GND | Pwr | Ground |
| 92 | BLANK | Out | Blanking Signal |
| 93 | BURST | Out | Color Burst Signal |
| 94 | PCLK | Out | Pixel Clock |
| 95 | VCC | Pwr | +5V Power |
| 96-100| ... | ... | (Power/Ground and specialized test pins) |

## Functional Responsibilities

- **VRAM Interface:** Manages a 16-bit wide data bus to VRAM (split into two 8-bit chips).
- **Backgrounds:** Renders up to 4 background layers (BG1-BG4) with various scroll positions and priorities.
- **Sprites:** Fetches sprite data from OAM (Object Attribute Memory) and manages sprite rendering.
- **Mode 7:** Performs the famous affine transformations (scaling/rotation) on BG1.
- **Interconnect:** Sends 8 bits of color index and 2 bits of priority to PPU2 for final color lookup and mixing.
