#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "tusb.h"

#include "pico_hdmi/video_output.h"
#include "pico_hdmi/hstx_data_island_queue.h"

#include "video/video_pipeline.h"
#include "video/video_capture.h"
#include "video/snes_timing.h"
#include "video/video_config.h"
#include "config.h"

#if ENABLE_AUDIO
#include "audio/audio_pipeline.h"
#endif

#include <stdio.h>

void __scratch_x("") vsync_callback(void) {
    line_ring_output_vsync();
#if ENABLE_AUDIO
    audio_pipeline_step();
#endif
}

int main(void)
{
    sleep_ms(1000);

    set_sys_clock_khz(126000, true);
    stdio_init_all();

    sleep_ms(500);
    stdio_flush();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    printf("\n\n=== SuperPico HDMI Mod ===\n");

    printf("Init video pipeline...\n");

    // Match neopico-hd init order: DI queue → video pipeline → background task → capture → Core 1
    hstx_di_queue_init();
    video_pipeline_init();

    printf("Init HDMI output...\n");
    video_output_init(FRAME_WIDTH, FRAME_HEIGHT);
    video_output_set_scanline_callback(scanline_callback);
    video_output_set_vsync_callback(vsync_callback);

#if ENABLE_AUDIO
    printf("Init audio pipeline...\n");
    audio_pipeline_init();
    video_output_set_background_task(audio_pipeline_process);
#endif

    printf("Init video capture...\n");
    video_capture_init(SNES_V_ACTIVE);
    sleep_ms(200);
    stdio_flush();

    printf("Launch Core 1 (HDMI)...\n");
    multicore_launch_core1(video_output_core1_run);
    sleep_ms(100);

    printf("Starting capture loop...\n");
    video_capture_run();

    return 0;
}
