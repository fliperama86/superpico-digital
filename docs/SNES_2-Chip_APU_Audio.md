# SNES APU (Audio Processing Unit) Subsystem

The SNES audio subsystem is a standalone computer within the console, designed by Sony. It consists of two main ICs, its own RAM, and a dedicated clock.

## S-SMP: Sony SPC700 (80-pin QFP)

The SPC700 is the CPU for the audio system. It executes code stored in its own ARAM and controls the S-DSP.

| Pin | Name | Description |
| :--- | :--- | :--- |
| 1-8 | D0-D7 | CPU Data Bus Interface (to 5A22 Bus B) |
| 9 | /CS | Chip Select (from 5A22) |
| 10 | /WR | Write Enable (from 5A22) |
| 11 | /RD | Read Enable (from 5A22) |
| 12-15 | PA0-PA3 | Address Bus B Bits 0-3 |
| 16 | RESET | System Reset |
| 17 | XIN | 24.576 MHz Clock Input |
| 18 | XOUT | Clock Output |
| 19-34 | MA0-MA15| Audio RAM Address Bus |
| 35-42 | MD0-MD7 | Audio RAM Data Bus |
| ... | ... | (Power and Ground pins) |

## S-DSP: Sony S-DSP (80-pin QFP)

The S-DSP is the 16-bit digital signal processor that handles sample decompression (BRR), volume, pitch, echo, and mixing of 8 audio channels.

| Signal | Description |
| :--- | :--- |
| **BRR Input** | Compressed sample data from ARAM |
| **Channel Mixing**| 8 independent voices with ADSR envelopes |
| **Echo Buffer** | Uses ARAM to store echo data |
| **L/R Output** | 16-bit digital audio output to the DAC |

## Audio DAC and Mixer

In 2-chip models, the digital output from the S-DSP is sent to a dedicated DAC (often a Sony CXD1222 or similar) and then through an analog mixer/filter stage (using op-amps like the LM324).

### Audio Signal Path:
1.  **S-SMP** (Control/Sequence) -> **S-DSP** (Synthesis).
2.  **S-DSP** -> **DAC** (Digital to Analog).
3.  **DAC** -> **Analog Filter/Amp** (Op-amp stage).
4.  **Analog stage** -> **Multi-Out** Pins 1 & 2.

## ARAM (Audio RAM)
- **Size:** 64 KB.
- **Type:** Often a single 64KB SRAM chip or 2x 32KB chips.
- **Purpose:** Stores the SPC700 program, sample data (BRR), and the echo buffer.
