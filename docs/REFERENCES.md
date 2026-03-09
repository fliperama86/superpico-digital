# SuperPico Digital — References & Credits

This document collects all external references, inspiration projects, and credits for the SuperPico Digital mod.

---

## SNES Hardware Documentation

- **SNESdev Wiki — PPU Pinout:** [https://snes.nesdev.org/wiki/PPU_pinout](https://snes.nesdev.org/wiki/PPU_pinout)
- **SNESdev Wiki — APU Pinout:** [https://snes.nesdev.org/wiki/APU_pinout](https://snes.nesdev.org/wiki/APU_pinout)
- **SNESdev Wiki — INIDISP ($2100):** [https://undisbeliever.net/snesdev/registers/inidisp.html](https://undisbeliever.net/snesdev/registers/inidisp.html)
- **Fullsnes (nocash) — Complete SNES Hardware Specs:** [https://problemkaputt.de/fullsnes.htm](https://problemkaputt.de/fullsnes.htm)
- **Super Famicom Development Wiki:** [https://wiki.superfamicom.org/](https://wiki.superfamicom.org/)
- **jwdonal Official SNES Schematics:** Available on gamesx.com wiki
- **1-CHIP PAL SNES Schematic:** [https://videogameperfection.com/forums/topic/schematic-for-1chip-pal-snes/](https://videogameperfection.com/forums/topic/schematic-for-1chip-pal-snes/)

---

## Prior Art & Inspiration

These projects were studied during development. SuperPico has a different architecture (RP2350B + HDMI) but shares the same core hardware insight — tapping the PPU2 TST pins.

- **SNES_TST (Opatusos — original):** [https://github.com/Opatusos/SNES_TST](https://github.com/Opatusos/SNES_TST)
  CPLD-based digital RGB capture from PPU2 TST pins. First known public implementation of the TST pin tap technique.

- **SNES_TST (michael-hirschmugl fork):** [https://github.com/michael-hirschmugl/SNES_TST](https://github.com/michael-hirschmugl/SNES_TST)
  Extended fork with dual analog output, Mode 7 patch, SuperCIC integration, and dejitter.

- **SNES MultiRegion with DeJitter (borti4938):** [https://github.com/borti4938/SNES_MultiRegion_with_DeJitter_QID](https://github.com/borti4938/SNES_MultiRegion_with_DeJitter_QID)
  CPLD-based dejitter and region switching reference.

- **neopico-hd (MVS):** Microcontroller-based HDMI capture for Neo Geo. Provided the RP2350 + pico_hdmi architectural template used in SuperPico, including the dual-core design, PIO capture strategy, pixel LUT, and audio pipeline structure.

- **SNES/PDIF Digital Audio (qwertymodo):** [https://www.qwertymodo.com/hardware-projects/snes/snespdif-digital-audio-mod](https://www.qwertymodo.com/hardware-projects/snes/snespdif-digital-audio-mod)
  Reference for S-DSP audio tap pinout and signal characteristics.

- **SuperCIC Project:** [https://sd2snes.de/blog/cool-stuff/supercic](https://sd2snes.de/blog/cool-stuff/supercic)
  Region-free CIC replacement reference.

---

## Community Research

- **Shmups.system11.org — SNES TST Pin Thread:** [https://shmups.system11.org/viewtopic.php?f=6&t=66597](https://shmups.system11.org/viewtopic.php?f=6&t=66597)
  Community investigation into PPU2 TST pin behavior and capture techniques.

- **Circuit-board.de — Decapped PPU Analysis:** [https://circuit-board.de/forum/index.php/Thread/25396](https://circuit-board.de/forum/index.php/Thread/25396)
  Die shots and internal analysis of PPU1 (5C77) and PPU2 (5C78).

- **INIDISP Brightness DAC measurements (lidnariq):** [https://forums.nesdev.org/viewtopic.php?p=257831#p257831](https://forums.nesdev.org/viewtopic.php?p=257831#p257831)

---

## RP2350 / Firmware

- **pico_hdmi library:** HSTX-based HDMI output for RP2350. Used as the video output backbone in SuperPico.
- **Raspberry Pi RP2350 Datasheet:** [https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
