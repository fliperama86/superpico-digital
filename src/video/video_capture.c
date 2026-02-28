#include "video_capture.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "snes_pins.h"
#include "snes_timing.h"
#include "video_capture.pio.h"
#include "video_pipeline.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// State
// =============================================================================

static uint32_t g_snes_height = 0;
static PIO g_pio_snes = pio1;

static uint g_sm_pixel = 0;
static uint g_offset_pixel = 0;
static pio_sm_config g_pio_config;

static int g_dma_chan = -1;
static uint32_t g_line_buffers[2][SNES_H_TOTAL];
static volatile uint32_t g_frame_count = 0;

// =============================================================================
// Pixel Conversion - RGB555 to RGB565 LUT
// =============================================================================
// PCB wires MSB (R4/G4/B4) to lower GPIO, so each 5-bit channel is bit-reversed.
// Same pattern as neopico-hd.

static inline uint32_t snes_reverse_5bit(uint32_t x) {
  return ((x & 1U) << 4) | ((x & 2U) << 2) | (x & 4U) | ((x & 8U) >> 2) |
         ((x & 16U) >> 4);
}

static inline uint16_t snes_pack_rgb565(uint32_t r5, uint32_t g5,
                                        uint32_t b5) {
  return (uint16_t)((r5 << 11) | (g5 << 6) | (g5 >> 4) | b5);
}

// 32K LUT: raw RGB555 (with reversed bits) -> corrected RGB565.
static uint16_t g_pixel_lut[32768] __attribute__((aligned(4)));

static void generate_pixel_lut(void) {
  for (uint32_t idx = 0; idx < 32768U; idx++) {
    uint32_t b5 = snes_reverse_5bit(idx & 0x1F);
    uint32_t g5 = snes_reverse_5bit((idx >> 5) & 0x1F);
    uint32_t r5 = snes_reverse_5bit((idx >> 10) & 0x1F);
    g_pixel_lut[idx] = snes_pack_rgb565(r5, g5, b5);
  }
}

// =============================================================================
// Internal Helpers
// =============================================================================

static void video_capture_reset_hardware(void) {
  pio_sm_set_enabled(g_pio_snes, g_sm_pixel, false);
  pio_sm_clear_fifos(g_pio_snes, g_sm_pixel);
  pio_sm_init(g_pio_snes, g_sm_pixel, g_offset_pixel, &g_pio_config);
  // Reapply IN_BASE after pio_sm_init (which resets pinctrl).
  // pin index 11 = GP27 (VBLANK) with GPIOBASE=16.
  uint pin_idx = PIN_SNES_BASE - 16;
  g_pio_snes->sm[g_sm_pixel].pinctrl =
      (g_pio_snes->sm[g_sm_pixel].pinctrl & ~0x000f8000u) | (pin_idx << 15);
  pio_sm_set_enabled(g_pio_snes, g_sm_pixel, true);
  pio_sm_put_blocking(g_pio_snes, g_sm_pixel, SNES_H_ACTIVE - 1);
}

// =============================================================================
// Public API
// =============================================================================

void video_capture_init(uint32_t height) {
  g_snes_height = height;
  generate_pixel_lut();

  pio_clear_instruction_memory(g_pio_snes);

  // RP2350: reset both PIO blocks, then force PIO1 GPIOBASE=16 via direct
  // register write (offset 0x168) so PIO can reach GP44 with 5-bit indices.
  // Pattern matches neopico-hd reference implementation.
  pio_set_gpio_base(pio0, 0);
  pio_set_gpio_base(pio1, 0);
  *(volatile uint32_t *)((uintptr_t)g_pio_snes + 0x168) = 16;

  g_offset_pixel = pio_add_program(g_pio_snes, &snes_hard_sync_program);
  g_sm_pixel = pio_claim_unused_sm(g_pio_snes, true);

  // Initialize all capture GPIOs GP27-GP44 (VBLANK, PCLK, B, G, R, HBLANK).
  for (uint pin = PIN_SNES_BASE; pin <= SNES_CAPTURE_PIN_LAST; pin++) {
    pio_gpio_init(g_pio_snes, pin);
    gpio_disable_pulls(pin);
    gpio_set_input_enabled(pin, true);
    gpio_set_input_hysteresis_enabled(pin, true);
  }

  g_pio_config = snes_hard_sync_program_get_default_config(g_offset_pixel);
  sm_config_set_clkdiv(&g_pio_config, 1.0f);
  // Autopush every 18 bits (one full capture word per pixel).
  sm_config_set_in_shift(&g_pio_config, false, true, SNES_CAPTURE_BITS);

  // video_capture_reset_hardware() calls pio_sm_init and applies IN_BASE.
  video_capture_reset_hardware();

  g_dma_chan = dma_claim_unused_channel(true);
  dma_channel_config dc = dma_channel_get_default_config(g_dma_chan);
  channel_config_set_read_increment(&dc, false);
  channel_config_set_write_increment(&dc, true);
  channel_config_set_dreq(&dc, pio_get_dreq(g_pio_snes, g_sm_pixel, false));
  dma_channel_configure(g_dma_chan, &dc, g_line_buffers[0],
                        &g_pio_snes->rxf[g_sm_pixel], SNES_H_ACTIVE, false);
}

void video_capture_run(void) {
  while (1) {
    // 1. Detect VSync Falling Edge (Active Video Start) in C
    while (!gpio_get(PIN_SNES_VBLANK))
      tight_loop_contents(); // Wait for Blanking
    while (gpio_get(PIN_SNES_VBLANK))
      tight_loop_contents(); // Wait for Active Video

    g_frame_count++;
    if (g_frame_count % 60 == 0) {
      gpio_xor_mask(1ul << PICO_DEFAULT_LED_PIN);
    }

    // 2. Align PIO and DMA to the start of the frame
    video_capture_reset_hardware();
    dma_channel_abort(g_dma_chan);
    dma_channel_set_trans_count(g_dma_chan, SNES_H_ACTIVE, false);
    dma_channel_set_write_addr(g_dma_chan, g_line_buffers[0], true);

    // Signal VSYNC to Core 1
    line_ring_vsync();

    // 3. Release PIO to start capturing lines
    pio_interrupt_clear(g_pio_snes, 4);
    pio_sm_exec(g_pio_snes, g_sm_pixel, pio_encode_irq_set(false, 4));

    for (uint16_t y = 0; y < g_snes_height; y++) {
      uint16_t *dst = line_ring_write_ptr(y);

      dma_channel_wait_for_finish_blocking(g_dma_chan);

      uint32_t *captured_buf = g_line_buffers[y % 2];
      if (y + 1 < g_snes_height) {
        dma_channel_set_trans_count(g_dma_chan, SNES_H_ACTIVE, false);
        dma_channel_set_write_addr(g_dma_chan, g_line_buffers[(y + 1) % 2],
                                   true);
      }

      for (int x = 0; x < SNES_H_ACTIVE; x++) {
        dst[x] = g_pixel_lut[(captured_buf[x] >> 2) & 0x7FFF];
      }

      line_ring_commit(y + 1);
    }
  }
}

uint32_t video_capture_get_frame_count(void) { return g_frame_count; }
