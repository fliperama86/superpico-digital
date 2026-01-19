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

static const uint16_t g_pixel_lut[4] = {0x0000, 0x0000, 0xFFFF, 0xFFFF};

// =============================================================================
// Internal Helpers
// =============================================================================

static void video_capture_reset_hardware(void) {
  pio_sm_set_enabled(g_pio_snes, g_sm_pixel, false);
  pio_sm_clear_fifos(g_pio_snes, g_sm_pixel);
  pio_sm_init(g_pio_snes, g_sm_pixel, g_offset_pixel, &g_pio_config);
  pio_sm_set_enabled(g_pio_snes, g_sm_pixel, true);
  pio_sm_put_blocking(g_pio_snes, g_sm_pixel, SNES_H_ACTIVE - 1);
}

// =============================================================================
// Public API
// =============================================================================

void video_capture_init(uint32_t height) {
  g_snes_height = height;

  pio_clear_instruction_memory(g_pio_snes);

  g_offset_pixel = pio_add_program(g_pio_snes, &snes_hard_sync_program);
  g_sm_pixel = pio_claim_unused_sm(g_pio_snes, true);

  uint pins[] = {PIN_SNES_PCLK, PIN_SNES_R0, PIN_SNES_HBLANK, PIN_SNES_VBLANK};
  for (int i = 0; i < 4; i++) {
    pio_gpio_init(g_pio_snes, pins[i]);
    gpio_set_dir(pins[i], GPIO_IN);
    gpio_set_input_enabled(pins[i], true);
    gpio_set_input_hysteresis_enabled(pins[i], true);
    gpio_disable_pulls(pins[i]);
  }

  g_pio_config = snes_hard_sync_program_get_default_config(g_offset_pixel);
  sm_config_set_in_pins(&g_pio_config, PIN_SNES_PCLK);
  sm_config_set_in_shift(&g_pio_config, false, true, 2);

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
        dst[x] = g_pixel_lut[captured_buf[x] & 0x3];
      }

      line_ring_commit(y + 1);
    }
  }
}

uint32_t video_capture_get_frame_count(void) { return g_frame_count; }
