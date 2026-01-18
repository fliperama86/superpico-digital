#include "video_capture.h"
#include "video_capture.pio.h"
#include "snes_timing.h"
#include "snes_pins.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <string.h>
#include <stdio.h>

// CSYNC Debug mode - no framebuffer
#define CSYNC_DEBUG_ONLY 1

// =============================================================================
// State
// =============================================================================

static uint32_t g_snes_height = 0;
static PIO g_pio_snes = pio1; // Use PIO1 to keep PIO0 free for freq counter later

static uint g_sm_sync = 0;
static uint g_offset_sync = 0;
static uint g_sm_pixel = 0;
static uint g_offset_pixel = 0;

static int g_dma_chan = -1;
static uint32_t g_line_buffer[SNES_H_TOTAL]; // Raw PIO words
static volatile uint32_t g_frame_count = 0;

// =============================================================================
// Pixel Conversion
// =============================================================================

// PIO captures [2:CSYNC][1:R0][0:PCLK]
// We just want R0 to show something on screen.
static inline uint16_t convert_pixel(uint32_t raw) {
    bool r0 = (raw >> 1) & 1;
    return r0 ? 0xFFFF : 0x0000; // White if R0 high, else Black
}

// =============================================================================
// Debug Helpers
// =============================================================================

static void debug_gpio_states(void) {
    printf("GPIO States: PCLK(GP%d)=%d  R0(GP%d)=%d  CSYNC(GP%d)=%d\n",
           PIN_SNES_PCLK, gpio_get(PIN_SNES_PCLK),
           PIN_SNES_R0, gpio_get(PIN_SNES_R0),
           PIN_SNES_CSYNC, gpio_get(PIN_SNES_CSYNC));
}

static void debug_pclk_activity(void) {
    // Sample PCLK rapidly to detect toggling
    int toggles = 0;
    bool last = gpio_get(PIN_SNES_PCLK);
    for (volatile int i = 0; i < 50000; i++) {
        bool curr = gpio_get(PIN_SNES_PCLK);
        if (curr != last) {
            toggles++;
            last = curr;
        }
    }
    printf("PCLK: %d toggles in 50k samples (%s)\n",
           toggles, toggles > 100 ? "ACTIVE" : "DEAD/SLOW");
}

static void debug_csync_activity(void) {
    // Sample CSYNC to see if it's toggling
    int toggles = 0;
    bool last = gpio_get(PIN_SNES_CSYNC);
    uint32_t start = time_us_32();
    while (time_us_32() - start < 50000) {  // 50ms sample
        bool curr = gpio_get(PIN_SNES_CSYNC);
        if (curr != last) {
            toggles++;
            last = curr;
        }
    }
    printf("CSYNC: %d edges in 50ms (%s)\n",
           toggles, toggles > 100 ? "ACTIVE" : "DEAD/SLOW");
}

static void debug_pio_fifo(PIO pio, uint sm) {
    uint level = pio_sm_get_rx_fifo_level(pio, sm);
    printf("PIO SM%d RX FIFO level: %d/4\n", sm, level);
}

// =============================================================================
// Sync Detection (Matching NeoPico-HD)
// =============================================================================

static bool wait_for_vsync(PIO pio, uint sm_sync, uint32_t timeout_ms) {
    uint32_t equ_count = 0;
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
    bool in_vsync = false;

    // Debug: Limit print rate
    static uint32_t print_cooldown = 0;
    static uint32_t debug_cycle = 0;
    bool can_print = (time_us_32() - print_cooldown > 1000000); // Print every 1s
    if (can_print) {
        print_cooldown = time_us_32();
        debug_cycle++;
        printf("\n--- Debug Cycle %lu ---\n", debug_cycle);
        debug_gpio_states();
        debug_pclk_activity();
        debug_csync_activity();
        debug_pio_fifo(pio, sm_sync);
    }

    int samples = 0;

    while (true) {
        if (absolute_time_diff_us(get_absolute_time(), timeout) <= 0) {
            if (can_print) printf("Timeout waiting for VSYNC (FIFO empty)\n");
            return false;
        }

        if (pio_sm_is_rx_fifo_empty(pio, sm_sync)) {
            tud_task(); // Ensure USB runs
            continue;
        }

        uint32_t h_ctr = pio_sm_get(pio, sm_sync);

        // Check for PIO Timeout (Stuck High)
        if (h_ctr == 0xFFFFFFFF) {
            if (can_print && samples < 5) {
                printf("Pulse: TIMEOUT (CSYNC stuck HIGH)\n");
                samples++;
            }
            continue;
        }

        // Normal measurement
        bool is_short_pulse = (h_ctr <= H_THRESHOLD);

        if (can_print && samples < 15) {
            printf("Pulse: %lu cycles (%s)\n", h_ctr, is_short_pulse ? "EQ/VSYNC" : "HSYNC");
            samples++;
        }

        if (!in_vsync) {
            if (is_short_pulse) {
                equ_count++;
            } else {
                if (equ_count >= 8) {
                    if (can_print) printf("VSYNC DETECTED (Pre-EQ count: %lu)\n", equ_count);
                    in_vsync = true;
                    equ_count = 0;
                }
                equ_count = 0;
            }
        } else {
            if (is_short_pulse) {
                equ_count++;
            } else {
                if (can_print) printf("VSYNC END\n");
                return true;
            }
        }
    }
}

// =============================================================================
// Public API
// =============================================================================

void video_capture_init(uint32_t height) {
    g_snes_height = height;

    // Initialize PIO
    g_offset_sync = pio_add_program(g_pio_snes, &snes_sync_program);
    g_offset_pixel = pio_add_program(g_pio_snes, &snes_pixel_capture_program);

    g_sm_sync = pio_claim_unused_sm(g_pio_snes, true);
    g_sm_pixel = pio_claim_unused_sm(g_pio_snes, true);

    // GPIO Setup
    pio_gpio_init(g_pio_snes, PIN_SNES_PCLK);
    pio_gpio_init(g_pio_snes, PIN_SNES_R0);
    pio_gpio_init(g_pio_snes, PIN_SNES_CSYNC);
    gpio_set_pulls(PIN_SNES_CSYNC, true, false); // Pull-up CSYNC

    // Sync SM Config
    pio_sm_config c_sync = snes_sync_program_get_default_config(g_offset_sync);
    sm_config_set_in_pins(&c_sync, PIN_SNES_CSYNC);
    sm_config_set_jmp_pin(&c_sync, PIN_SNES_CSYNC);
    sm_config_set_in_shift(&c_sync, false, false, 32);
    pio_sm_init(g_pio_snes, g_sm_sync, g_offset_sync, &c_sync);

    // Pixel SM Config
    pio_sm_config c_pixel = snes_pixel_capture_program_get_default_config(g_offset_pixel);
    sm_config_set_in_pins(&c_pixel, PIN_SNES_PCLK); // Base is PCLK
    sm_config_set_in_shift(&c_pixel, false, true, 3); // 3 bits per pixel
    pio_sm_init(g_pio_snes, g_sm_pixel, g_offset_pixel, &c_pixel);

    // Start SMs
    pio_sm_set_enabled(g_pio_snes, g_sm_sync, true);
    pio_sm_set_enabled(g_pio_snes, g_sm_pixel, true);

    // Feed total pixel count
    pio_sm_put_blocking(g_pio_snes, g_sm_pixel, SNES_H_TOTAL - 1);

    // DMA Setup
    g_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config dc = dma_channel_get_default_config(g_dma_chan);
    channel_config_set_read_increment(&dc, false);
    channel_config_set_write_increment(&dc, true);
    channel_config_set_dreq(&dc, pio_get_dreq(g_pio_snes, g_sm_pixel, false));
    dma_channel_configure(g_dma_chan, &dc, g_line_buffer, &g_pio_snes->rxf[g_sm_pixel], 0, false);
}

// Frame buffer for single frame capture (1 bit per pixel, packed)
// 256 pixels / 8 = 32 bytes per line, 224 lines = 7168 bytes
#define FRAME_BYTES_PER_LINE (SNES_H_ACTIVE / 8)
#define FRAME_TOTAL_BYTES (FRAME_BYTES_PER_LINE * SNES_V_ACTIVE)
static uint8_t g_frame_data[FRAME_TOTAL_BYTES];

static bool capture_single_frame(void) {
    // Wait for VSYNC
    printf("Waiting for VSYNC...\n");
    if (!wait_for_vsync(g_pio_snes, g_sm_sync, 500)) {
        printf("VSYNC timeout!\n");
        return false;
    }
    printf("Got VSYNC, triggering pixel capture...\n");

    // Reset pixel SM - drain FIFO first
    while (!pio_sm_is_rx_fifo_empty(g_pio_snes, g_sm_pixel)) {
        pio_sm_get(g_pio_snes, g_sm_pixel);
    }

    // Trigger pixel capture by setting IRQ 4
    // SM is waiting at "wait 1 irq 4", setting it will release
    pio_interrupt_clear(g_pio_snes, 4);
    pio_sm_set_enabled(g_pio_snes, g_sm_pixel, false);
    pio_sm_clear_fifos(g_pio_snes, g_sm_pixel);
    pio_sm_restart(g_pio_snes, g_sm_pixel);
    pio_sm_exec(g_pio_snes, g_sm_pixel, pio_encode_jmp(g_offset_pixel)); // Back to start
    pio_sm_put_blocking(g_pio_snes, g_sm_pixel, SNES_H_TOTAL - 1); // Re-feed count
    pio_sm_set_enabled(g_pio_snes, g_sm_pixel, true);
    pio_interrupt_clear(g_pio_snes, 4);
    pio_sm_exec(g_pio_snes, g_sm_pixel, pio_encode_irq_set(false, 4)); // Set IRQ 4 to trigger

    printf("Skipping %d vblank lines...\n", SNES_V_SKIP_LINES);

    // Check pixel SM FIFO level
    sleep_ms(50);  // Give it time to start producing
    printf("Pixel SM FIFO level: %d\n", pio_sm_get_rx_fifo_level(g_pio_snes, g_sm_pixel));

    // Skip vertical blanking lines with timeout
    for (int i = 0; i < SNES_V_SKIP_LINES; i++) {
        // Reset DMA write address and start transfer
        dma_channel_set_write_addr(g_dma_chan, g_line_buffer, false);
        dma_channel_set_trans_count(g_dma_chan, SNES_H_TOTAL, true);

        // Wait with timeout
        uint32_t start = time_us_32();
        while (dma_channel_is_busy(g_dma_chan)) {
            if (time_us_32() - start > 100000) {  // 100ms timeout
                printf("DMA timeout on vblank line %d! FIFO=%d\n",
                       i, pio_sm_get_rx_fifo_level(g_pio_snes, g_sm_pixel));
                return false;
            }
        }
        if (i == 0) printf("First vblank line done\n");
    }

    printf("Capturing %d active lines...\n", SNES_V_ACTIVE);

    // Capture active lines
    for (uint32_t y = 0; y < SNES_V_ACTIVE; y++) {
        dma_channel_set_write_addr(g_dma_chan, g_line_buffer, false);
        dma_channel_set_trans_count(g_dma_chan, SNES_H_TOTAL, true);
        dma_channel_wait_for_finish_blocking(g_dma_chan);

        // Pack R0 bits into bytes (8 pixels per byte)
        uint8_t *line_out = &g_frame_data[y * FRAME_BYTES_PER_LINE];
        for (int bx = 0; bx < FRAME_BYTES_PER_LINE; bx++) {
            uint8_t byte = 0;
            for (int bit = 0; bit < 8; bit++) {
                int x = bx * 8 + bit;
                uint32_t raw = g_line_buffer[SNES_H_SKIP_START + x];
                bool r0 = (raw >> 1) & 1;
                if (r0) byte |= (0x80 >> bit);  // MSB first
            }
            line_out[bx] = byte;
        }

        // Progress every 32 lines
        if (y % 32 == 0) {
            printf("  Line %lu/%d\n", y, SNES_V_ACTIVE);
        }
    }

    printf("Capture complete!\n");
    return true;
}

static void send_frame_data(void) {
    // Send sync header
    printf("FRAME_START:%d:%d\n", SNES_H_ACTIVE, SNES_V_ACTIVE);

    // Small delay to let header flush
    for (int i = 0; i < 5; i++) { tud_task(); sleep_ms(10); }

    // Send binary data in chunks with tud_task() calls
    const int chunk_size = 64;
    for (int i = 0; i < FRAME_TOTAL_BYTES; i += chunk_size) {
        int remaining = FRAME_TOTAL_BYTES - i;
        int to_send = (remaining < chunk_size) ? remaining : chunk_size;

        // Write raw bytes
        for (int j = 0; j < to_send; j++) {
            putchar(g_frame_data[i + j]);
        }
        tud_task();
    }

    // Send sync footer
    sleep_ms(10);
    tud_task();
    printf("\nFRAME_END\n");
}

void video_capture_run(void) {
    printf("Frame capture mode ready.\n");
    printf("Send 'C' to capture a frame.\n");

    while (1) {
        // Heartbeat LED
        static uint32_t last_blink = 0;
        if (time_us_32() - last_blink > 500000) {
            gpio_xor_mask(1ul << PICO_DEFAULT_LED_PIN);
            last_blink = time_us_32();
        }

        tud_task();

        // Check for serial command
        int c = getchar_timeout_us(1000);
        if (c == 'C' || c == 'c') {
            printf("Capturing frame...\n");

            // Delay to let serial settle
            for (int i = 0; i < 20; i++) { tud_task(); sleep_ms(10); }

            if (capture_single_frame()) {
                printf("Frame captured. Sending...\n");
                for (int i = 0; i < 10; i++) { tud_task(); sleep_ms(10); }
                send_frame_data();
                printf("Done.\n");
            } else {
                printf("ERROR: Failed to capture (no VSYNC)\n");
            }
        }
    }
}

uint32_t video_capture_get_frame_count(void) {
    return g_frame_count;
}
