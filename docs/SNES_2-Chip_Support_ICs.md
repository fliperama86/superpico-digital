# SNES Support ICs: MAD-1 and CIC

These auxiliary chips handle memory mapping and region lockout, which are essential for the operation of any SNES console.

## 1. MAD-1 (Memory Address Decoder)

The MAD-1 is a custom Nintendo chip used to decode the CPU's high address lines (A15-A23) to select between ROM, WRAM, and other peripherals. It is found on many cartridges and also on the motherboard (as the S-WRAM or a discrete MAD-1).

### Pinout (SOP-16)

| Pin | Name | Description |
| :--- | :--- | :--- |
| 1 | A15 / A20 | Address Input |
| 2 | A12 / A21 | Address Input |
| 3 | A13 / A22 | Address Input |
| 4 | /CART | Cartridge Select Input |
| 5 | /RD | Read Enable Input |
| 6 | /WR | Write Enable Input |
| 7 | /ROM_CS | ROM Chip Select Output |
| 8 | GND | Ground |
| 9 | /WRAM_CS | WRAM Chip Select Output |
| 10 | VCC | +5V Power |
| 11 | VCC | +5V Power |
| 12 | /RAM_CS | SRAM (Cartridge) Chip Select Output |
| 13 | /HI | ROM High Bank Select |
| 14 | /LO | ROM Low Bank Select |
| 15 | /RESET | System Reset Input |
| 16 | /OE | Output Enable |

## 2. CIC (Checking Integrated Circuit)

The CIC (F411/D411 for NTSC, F413/D413 for PAL) is a security micro-controller. A handshake must occur between the "lock" CIC in the console and the "key" CIC in the cartridge, or the console will be held in a permanent reset state.

### Pinout (DIP-16/SOP-16)

| Pin | Name | Description |
| :--- | :--- | :--- |
| 1 | DATA | Serial Data (to Key) |
| 2 | CLK | Clock Output (to Key) |
| 3 | /LOCK | Lock Indicator |
| 4 | /RESET_IN | Reset Signal from Button |
| 5 | GND | Ground |
| 6 | /RESET_OUT | Reset Signal to System (CPU/PPU) |
| 7 | VCC | +5V Power |
| 8-16| ... | (Internal test and manufacturing pins) |

## 3. Reset Supervisor Logic

In 2-chip models, the reset logic is often managed by a dedicated voltage supervisor (like the PST518) or a discrete transistor circuit.

- **Threshold:** If the 5V rail drops below ~4.2V, the supervisor pulls the `/RESET` line low.
- **Handshake:** The CIC controls the final state of the `/RESET` line to the CPU and PPUs. If the CIC handshake fails, it toggles the `/RESET` line at ~1Hz, causing the "blinking light" or no-boot behavior.
