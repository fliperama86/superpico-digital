/**
 * superpico-digital
 *
 * Digital-to-digital HDMI mod for SNES 2-chip consoles
 * using Raspberry Pi Pico 2 (RP2350) and the pico_hdmi library.
 *
 * Target: RP2350 (Raspberry Pi Pico 2)
 */

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "pico_hdmi/video_output.h"

#include "video/video_pipeline.h"
#include "video/freq_counter.h"
#include "audio/audio_pipeline.h"
#include "osd/osd.h"

#include <stdio.h>

// ============================================================================
// Configuration
// ============================================================================

// (Moved to headers mostly)

// ============================================================================
// Frequency Display Helper
// ============================================================================

static void format_freq(char *buf, size_t len, uint32_t freq_hz)
{
    if (freq_hz == 0) {
        snprintf(buf, len, "-- Hz");
    } else if (freq_hz >= 1000000) {
        // MHz range
        uint32_t mhz = freq_hz / 1000000;
        uint32_t khz_frac = (freq_hz % 1000000) / 10000;  // 2 decimal places
        snprintf(buf, len, "%lu.%02luMHz", mhz, khz_frac);
    } else if (freq_hz >= 1000) {
        // kHz range
        uint32_t khz = freq_hz / 1000;
        uint32_t hz_frac = (freq_hz % 1000) / 10;  // 2 decimal places
        snprintf(buf, len, "%lu.%02lukHz", khz, hz_frac);
    } else {
        // Hz range
        snprintf(buf, len, "%luHz", freq_hz);
    }
}

static void update_osd_frequencies(void)
{
    char buf[24];

    // Clear frequency area (lines 24-64)
    for (int y = 24; y < 72; y++) {
        for (int x = 0; x < OSD_BOX_W; x++) {
            osd_framebuffer[y][x] = OSD_COLOR_BG;
        }
    }

    // PCLK
    osd_puts(16, 24, "PCLK:", OSD_COLOR_FG);
    format_freq(buf, sizeof(buf), freq_pclk_hz);
    osd_puts(72, 24, buf, OSD_COLOR_TITLE);

    // R0
    osd_puts(16, 40, "R0:", OSD_COLOR_FG);
    format_freq(buf, sizeof(buf), freq_r0_hz);
    osd_puts(72, 40, buf, OSD_COLOR_TITLE);

    // CSYNC
    osd_puts(16, 56, "CSYNC:", OSD_COLOR_FG);
    format_freq(buf, sizeof(buf), freq_csync_hz);
    osd_puts(72, 56, buf, OSD_COLOR_TITLE);
}

// ============================================================================
// Main (Core 0)
// ============================================================================

int main(void)
{
    // Set system clock to 126 MHz for HSTX timing
    set_sys_clock_khz(126000, true);

    stdio_init_all();

    // Initialize LED for heartbeat
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1); // Turn ON immediately to prove life

    sleep_ms(1000);

    // Initialize subsystems
    video_pipeline_init();
    audio_pipeline_init();

    // Initialize OSD
    osd_init();
    osd_puts(16, 8, "SuperPico Digital", OSD_COLOR_TITLE);

    // Initialize frequency counter (DISABLED FOR DEBUGGING)
    // freq_counter_init();

    // Initialize HDMI output
    video_output_init(FRAME_WIDTH, FRAME_HEIGHT);

    // Register scanline callback
    video_output_set_scanline_callback(scanline_callback);

    // Launch Core 1 for HSTX output
    multicore_launch_core1(video_output_core1_run);
    sleep_ms(100);

    // Main loop - audio + LED heartbeat + frequency updates
    uint32_t last_frame = 0;
    bool led_state = true;

    while (1) {
        // Keep audio buffer fed
        audio_pipeline_process();

        while (video_frame_count == last_frame) {
            audio_pipeline_process();
            tight_loop_contents();
        }
        last_frame = video_frame_count;

        // Advance melody (one note step per frame)
        audio_pipeline_step();

        // Update frequency measurements and OSD
        // if (freq_counter_update()) {
        //     update_osd_frequencies();
        // }

        // LED heartbeat (FAST BLINK to show activity)
        if ((video_frame_count % 15) == 0) {
            led_state = !led_state;
            gpio_put(PICO_DEFAULT_LED_PIN, led_state);
        }
    }

    return 0;
}
