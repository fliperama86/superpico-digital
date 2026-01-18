/**
 * superpico-digital
 *
 * Digital-to-digital HDMI mod for SNES 2-chip consoles
 * using Raspberry Pi Pico 2 (RP2350) and the pico_hdmi library.
 *
 * Target: RP2350 (Raspberry Pi Pico 2)
 */

#include "pico_hdmi/hstx_data_island_queue.h"
#include "pico_hdmi/hstx_packet.h"
#include "pico_hdmi/video_output.h"

#include "freq_counter.h"
#include "osd/osd.h"

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/clocks.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Configuration
// ============================================================================

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 240

// Virtual video buffer (simulating a captured line)
static uint16_t scanline_buffer[VIDEO_WIDTH];

// Background colors (RGB565)
#define BG_COLOR 0x0010  // Dark blue

// Audio configuration
#define AUDIO_SAMPLE_RATE 48000
#define TONE_AMPLITUDE 6000

// ============================================================================
// Audio State
// ============================================================================

#define SINE_TABLE_SIZE 256
static int16_t sine_table[SINE_TABLE_SIZE];
static uint32_t audio_phase = 0;
static uint32_t phase_increment = 0;
static int audio_frame_counter = 0;

// Simple melody - C major scale
typedef struct {
    uint16_t freq;
    uint8_t duration;
} note_t;

static const note_t melody[] = {
    {262, 15}, // C4
    {294, 15}, // D4
    {330, 15}, // E4
    {349, 15}, // F4
    {392, 15}, // G4
    {440, 15}, // A4
    {494, 15}, // B4
    {523, 15}, // C5
    {0, 30},   // Rest
};
#define MELODY_LENGTH (sizeof(melody) / sizeof(melody[0]))

static int melody_index = 0;
static int note_frames_remaining = 0;

static void init_sine_table(void)
{
    for (int i = 0; i < SINE_TABLE_SIZE; i++) {
        float angle = (float)i * 2.0F * 3.14159265F / SINE_TABLE_SIZE;
        sine_table[i] = (int16_t)(sinf(angle) * TONE_AMPLITUDE);
    }
}

static void advance_melody(void)
{
    if (--note_frames_remaining <= 0) {
        melody_index = (melody_index + 1) % MELODY_LENGTH;

        note_frames_remaining = melody[melody_index].duration;
        uint16_t freq = melody[melody_index].freq;
        if (freq > 0) {
            phase_increment = (uint32_t)(((uint64_t)freq << 32) / AUDIO_SAMPLE_RATE);
        } else {
            phase_increment = 0; // Rest
        }
    }
}

static inline int16_t get_sine_sample(void)
{
    if (phase_increment == 0)
        return 0; // Rest
    int16_t s = sine_table[(audio_phase >> 24) & 0xFF];
    audio_phase += phase_increment;
    return s;
}

static void generate_audio(void)
{
    // Keep the audio queue fed
    while (hstx_di_queue_get_level() < 200) {
        audio_sample_t samples[4];
        for (int i = 0; i < 4; i++) {
            int16_t s = get_sine_sample();
            samples[i].left = s;
            samples[i].right = s;
        }

        hstx_packet_t packet;
        audio_frame_counter = hstx_packet_set_audio_samples(&packet, samples, 4, audio_frame_counter);

        hstx_data_island_t island;
        hstx_encode_data_island(&island, &packet, false, true);
        hstx_di_queue_push(&island);
    }
}

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
// Scanline Callback (runs on Core 1)
// ============================================================================

/**
 * Fast 2x pixel doubling: reads 2 pixels, writes 2 doubled words
 * Processes 32-bits at a time for efficiency
 */
static inline void __scratch_x("")
double_pixels_fast(uint32_t *dst, const uint16_t *src, int count) {
    const uint32_t *src32 = (const uint32_t *)src;
    int pairs = count / 2;

    for (int i = 0; i < pairs; i++) {
        uint32_t two = src32[i];
        uint32_t p0 = two & 0xFFFF;
        uint32_t p1 = two >> 16;
        dst[i * 2] = p0 | (p0 << 16);
        dst[i * 2 + 1] = p1 | (p1 << 16);
    }
}

void __scratch_x("") scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst)
{
    (void)v_scanline;

    // Check if OSD is visible AND this line intersects OSD box
    if (osd_visible && active_line >= OSD_BOX_Y && active_line < OSD_BOX_Y + OSD_BOX_H) {
        // OSD line within the OSD box
        uint32_t osd_line = active_line - OSD_BOX_Y;
        const uint16_t *osd_src = osd_framebuffer[osd_line];

        // === Loop splitting: 3 regions, no per-pixel branching ===

        // Region 1: Before OSD box (0 to OSD_BOX_X)
        // Read OSD_BOX_X/2 source pixels, write OSD_BOX_X output pixels
        double_pixels_fast(dst, scanline_buffer, OSD_BOX_X / 2);

        // Region 2: OSD box (OSD_BOX_X to OSD_BOX_X + OSD_BOX_W)
        // Copy OSD pixels (already RGB565, pack as uint32_t pairs)
        // OSD overlay is 1:1, not doubled
        const uint32_t *osd32 = (const uint32_t *)osd_src;
        for (int i = 0; i < OSD_BOX_W / 2; i++) {
            dst[OSD_BOX_X / 2 + i] = osd32[i];
        }

        // Region 3: After OSD box (OSD_BOX_X + OSD_BOX_W to FRAME_WIDTH)
        // Offset src by: OSD_BOX_X/2 + OSD_BOX_W/2 (input pixels skipped under OSD)
        // Offset dst by: (OSD_BOX_X + OSD_BOX_W)/2 (output uint32 indices)
        int src_offset = (OSD_BOX_X + OSD_BOX_W) / 2;
        int dst_offset = (OSD_BOX_X + OSD_BOX_W) / 2;
        int remaining_out_width = FRAME_WIDTH - OSD_BOX_X - OSD_BOX_W;

        double_pixels_fast(dst + dst_offset, 
                           scanline_buffer + src_offset, 
                           remaining_out_width / 2);
    } else {
        // Fast path: no OSD on this line, full video doubling
        double_pixels_fast(dst, scanline_buffer, VIDEO_WIDTH);
    }
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

    sleep_ms(1000);

    // Initialize audio
    init_sine_table();
    note_frames_remaining = melody[0].duration;
    phase_increment = (uint32_t)(((uint64_t)melody[0].freq << 32) / AUDIO_SAMPLE_RATE);

    // Initialize scanline buffer with a gradient/pattern
    // 320 pixels: Red -> Green gradient
    for (int i = 0; i < VIDEO_WIDTH; i++) {
        uint8_t r = (i * 31) / VIDEO_WIDTH;
        uint8_t g = 63 - ((i * 63) / VIDEO_WIDTH);
        uint8_t b = 15;
        scanline_buffer[i] = (r << 11) | (g << 5) | b;
    }

    // Initialize OSD
    osd_init();
    osd_puts(16, 8, "SuperPico Digital", OSD_COLOR_TITLE);

    // Initialize frequency counter
    freq_counter_init();

    // Initialize HDMI output
    hstx_di_queue_init();
    video_output_init(FRAME_WIDTH, FRAME_HEIGHT);

    // Register scanline callback
    video_output_set_scanline_callback(scanline_callback);

    // Pre-fill audio buffer
    generate_audio();

    // Launch Core 1 for HSTX output
    multicore_launch_core1(video_output_core1_run);
    sleep_ms(100);

    // Main loop - audio + LED heartbeat + frequency updates
    uint32_t last_frame = 0;
    bool led_state = false;

    while (1) {
        // Keep audio buffer fed
        generate_audio();

        while (video_frame_count == last_frame) {
            generate_audio();
            tight_loop_contents();
        }
        last_frame = video_frame_count;

        // Advance melody (one note step per frame)
        advance_melody();

        // Update frequency measurements and OSD
        if (freq_counter_update()) {
            update_osd_frequencies();
        }

        // LED heartbeat
        if ((video_frame_count % 30) == 0) {
            led_state = !led_state;
            gpio_put(PICO_DEFAULT_LED_PIN, led_state);
        }
    }

    return 0;
}
