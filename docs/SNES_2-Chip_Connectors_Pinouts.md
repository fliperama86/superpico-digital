# SNES External Connectors and Expansion Pinouts

## 1. Cartridge Connector (62-pin)

The cartridge port provides access to Address Bus A, Data Bus, and various control signals.

| Pin | Name | Pin | Name |
| :--- | :--- | :--- | :--- |
| 1 | +5V | 32 | +5V |
| 2 | /IRQ | 33 | /ROM (OE) |
| 3 | A16 | 34 | A17 |
| 4 | A15 | 35 | A18 |
| 5 | A14 | 36 | A19 |
| 6 | A13 | 37 | A20 |
| 7 | A12 | 38 | A21 |
| 8 | A11 | 39 | A22 |
| 9 | A10 | 40 | A23 |
| 10 | A9 | 41 | /RD |
| 11 | A8 | 42 | /WR |
| 12 | A7 | 43 | D0 |
| 13 | A6 | 44 | D1 |
| 14 | A5 | 45 | D2 |
| 15 | A4 | 46 | D3 |
| 16 | A3 | 47 | D4 |
| 17 | A2 | 48 | D5 |
| 18 | A1 | 49 | D6 |
| 19 | A0 | 50 | D7 |
| 20 | /CART (Insert)| 51 | /PAWR |
| 21 | D0 (Bus B) | 52 | /PARD |
| 22 | PA0 | 53 | PA1 |
| 23 | PA2 | 54 | PA3 |
| 24 | PA4 | 55 | PA5 |
| 25 | PA6 | 56 | PA7 |
| 26 | /RESET | 57 | SYSTEM CLOCK |
| 27 | AUDIO L | 58 | AUDIO R |
| 28 | AUDIO IN | 59 | REFRESH |
| 29 | +5V | 60 | +5V |
| 30 | GND | 61 | GND |
| 31 | GND | 62 | GND |

## 2. Multi-Out Connector (12-pin)

| Pin | Name | Description |
| :--- | :--- | :--- |
| 1 | AUDIO L | Analog Audio Left |
| 2 | AUDIO R | Analog Audio Right |
| 3 | GND | Ground |
| 4 | VIDEO (CVBS) | Composite Video |
| 5 | GND | Ground |
| 6 | Y | Luminance (S-Video) |
| 7 | C | Chrominance (S-Video) |
| 8 | BLUE | Analog RGB Blue |
| 9 | CVBS/CSYNC | Sync (Sync-on-Composite or CSYNC) |
| 10 | +5V | DC Power Output |
| 11 | RED | Analog RGB Red |
| 12 | GREEN | Analog RGB Green |

## 3. Expansion Port (EXT - Bottom)

The expansion port (used for the Satellaview in Japan) is essentially a breakout of Address Bus B and the I/O lines.

| Signals | Description |
| :--- | :--- |
| **Address Bus B** | PA0-PA7 |
| **Data Bus** | D0-D7 |
| **Control** | /PARD, /PAWR, /RESET, /IRQ |
| **Power** | +5V, +12V (Optional), GND |
| **I/O** | OUT0, OUT1, OUT2 (from CPU pins 71-73) |
