/**
 * Audio Pipeline - SNES I2S Capture → HDMI
 *
 * Captures ~32kHz audio from S-DSP via PIO/DMA, upsamples to 48kHz
 * via linear interpolation SRC, and feeds to HDMI data island queue.
 *
 * Modeled after neopico-hd's audio_subsystem.c.
 */

#include "audio_pipeline.h"
#include "config.h"

#if ENABLE_AUDIO
#include "pico/stdlib.h"
#include "pico_hdmi/hstx_data_island_queue.h"
#include "pico_hdmi/hstx_packet.h"
#include "pico_hdmi/video_output.h"

#include "audio_buffer.h"
#include "audio_common.h"
#include "i2s_capture.h"
#include "src.h"
#include "snes_pins.h"

#include <string.h>

#define PROCESS_BUFFER_SIZE 64

// State machine — matches neopico-hd's pattern
enum {
    AUDIO_STATE_WAIT_HSTX,  // Wait for HSTX to stabilize
    AUDIO_STATE_INIT,       // Initialize PIO + DMA
    AUDIO_STATE_WARM,       // Capture running, output muted
    AUDIO_STATE_RUNNING,    // Normal operation
};

#define AUDIO_HSTX_SETTLE_FRAMES 120  // ~2s at 60fps
#define AUDIO_WARM_FRAMES         30  // ~0.5s muted warmup

// Audio collect buffer (matches neopico-hd pattern)
#define AUDIO_COLLECT_SIZE 128

static struct {
    ap_ring_t capture_ring;
    i2s_capture_t capture;
    src_t src;

    // HDMI output
    int audio_frame_counter;
    audio_sample_t collect_buffer[AUDIO_COLLECT_SIZE];
    uint32_t collect_count;
    bool output_muted;

    // State machine
    int state;
    uint32_t state_enter_frame;

    uint32_t samples_output;
    bool initialized;
} g_pipeline;

static const audio_sample_t audio_silence[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

// Drift compensation — adjust SRC input rate based on DI queue level
// (matches neopico-hd's audio_background_task_control)
#define DRIFT_CHECK_INTERVAL 30  // frames (~0.5s at 60fps)
#define DRIFT_RATE_STEP      10  // Hz adjustment per check
#define DRIFT_QUEUE_TARGET  128  // half of 256
#define DRIFT_QUEUE_HIGH    160
#define DRIFT_QUEUE_LOW      96
#define DRIFT_RATE_MIN    30000  // SNES ~32kHz ±5%
#define DRIFT_RATE_MAX    34000

static uint32_t last_drift_frame;

static void drift_compensate(void)
{
    if (video_frame_count - last_drift_frame < DRIFT_CHECK_INTERVAL)
        return;
    last_drift_frame = video_frame_count;

    uint32_t level = hstx_di_queue_get_level();
    uint32_t rate = g_pipeline.src.input_rate;

    if (level > DRIFT_QUEUE_HIGH)
        rate += DRIFT_RATE_STEP;  // Queue filling up — speed up consumption
    else if (level < DRIFT_QUEUE_LOW)
        rate -= DRIFT_RATE_STEP;  // Queue draining — slow down consumption

    if (rate < DRIFT_RATE_MIN) rate = DRIFT_RATE_MIN;
    if (rate > DRIFT_RATE_MAX) rate = DRIFT_RATE_MAX;

    g_pipeline.src.input_rate = rate;
}

// Output callback — pushes SRC output samples to DI queue
static void audio_output_callback(const audio_sample_t *samples, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        if (g_pipeline.collect_count < AUDIO_COLLECT_SIZE)
            g_pipeline.collect_buffer[g_pipeline.collect_count++] = samples[i];

        if (g_pipeline.collect_count >= 4) {
            hstx_packet_t packet;
            const audio_sample_t *src = g_pipeline.output_muted
                ? audio_silence
                : g_pipeline.collect_buffer;
            int fc = hstx_packet_set_audio_samples(&packet, src, 4,
                                                   g_pipeline.audio_frame_counter);
            hstx_data_island_t island;
            hstx_encode_data_island(&island, &packet, false, true);

            if (hstx_di_queue_push(&island)) {
                g_pipeline.audio_frame_counter = fc;
                g_pipeline.collect_count -= 4;
                for (uint32_t j = 0; j < g_pipeline.collect_count; j++)
                    g_pipeline.collect_buffer[j] = g_pipeline.collect_buffer[j + 4];
                g_pipeline.samples_output += 4;
            } else {
                break;  // Queue full — don't block
            }
        }
    }
}

// Process captured audio through SRC (32kHz → 48kHz)
static void audio_do_process(void)
{
    i2s_capture_poll(&g_pipeline.capture);

    uint32_t available = ap_ring_available(&g_pipeline.capture_ring);
    if (available == 0)
        return;

    if (available > PROCESS_BUFFER_SIZE)
        available = PROCESS_BUFFER_SIZE;

    audio_sample_t in_buf[PROCESS_BUFFER_SIZE];
    for (uint32_t i = 0; i < available; i++)
        in_buf[i] = ap_ring_read(&g_pipeline.capture_ring);

    // SRC: 32kHz → 48kHz (ratio ~1.5x, so output can be up to 1.5x input + 1)
    audio_sample_t out_buf[PROCESS_BUFFER_SIZE * 2];
    uint32_t in_consumed;
    uint32_t out_count = src_process(&g_pipeline.src, in_buf, available,
                                      out_buf, PROCESS_BUFFER_SIZE * 2, &in_consumed);

    if (out_count > 0)
        audio_output_callback(out_buf, out_count);
}

static void audio_hw_init(void)
{
    ap_ring_init(&g_pipeline.capture_ring);

    i2s_capture_config_t cap_config = {
        .pin_bck = PIN_AUDIO_BCLK,
        .pin_dat = PIN_AUDIO_SDATA,
        .pin_ws = PIN_AUDIO_LRCK,
        .pio = pio0,
        .sm = 2,
    };

    if (!i2s_capture_init(&g_pipeline.capture, &cap_config, &g_pipeline.capture_ring))
        return;

    // SRC: 32040 Hz (SNES S-DSP) → 48000 Hz (HDMI default)
    src_init(&g_pipeline.src, SRC_INPUT_RATE_DEFAULT, SRC_OUTPUT_RATE_DEFAULT);
    src_set_mode(&g_pipeline.src, SRC_MODE_LINEAR);
}

void audio_pipeline_init(void)
{
    memset(&g_pipeline, 0, sizeof(g_pipeline));
    g_pipeline.state = AUDIO_STATE_WAIT_HSTX;
    g_pipeline.output_muted = true;
    g_pipeline.initialized = true;
}

// Background task — called from Core 1 via video_output_set_background_task.
// Must return quickly to avoid starving HDMI output.
void __scratch_x("") audio_pipeline_process(void)
{
    if (!g_pipeline.initialized)
        return;

    switch (g_pipeline.state) {
        case AUDIO_STATE_WAIT_HSTX:
            if (video_frame_count >= AUDIO_HSTX_SETTLE_FRAMES)
                g_pipeline.state = AUDIO_STATE_INIT;
            break;

        case AUDIO_STATE_INIT:
            audio_hw_init();
            i2s_capture_start(&g_pipeline.capture);
            g_pipeline.state_enter_frame = video_frame_count;
            g_pipeline.state = AUDIO_STATE_WARM;
            break;

        case AUDIO_STATE_WARM:
            audio_do_process();
            if (video_frame_count - g_pipeline.state_enter_frame >= AUDIO_WARM_FRAMES) {
                ap_ring_init(&g_pipeline.capture_ring);
                g_pipeline.collect_count = 0;
                g_pipeline.src.accumulator = 0;
                g_pipeline.src.phase = 0;
                g_pipeline.src.have_prev = false;
                last_drift_frame = video_frame_count;
                g_pipeline.output_muted = false;
                g_pipeline.state = AUDIO_STATE_RUNNING;
            }
            break;

        case AUDIO_STATE_RUNNING:
            drift_compensate();
            audio_do_process();
            while (ap_ring_available(&g_pipeline.capture_ring) > 0)
                audio_do_process();
            break;
    }
}

void audio_pipeline_step(void)
{
}
#endif
