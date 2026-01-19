#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "tusb.h"

#include "pico_hdmi/video_output.h"
#include "pico_hdmi/hstx_data_island_queue.h"

#include "video/video_pipeline.h"
#include "video/video_capture.h"
#include "video/snes_timing.h"

#include <stdio.h>

void __scratch_x("") vsync_callback(void) {
    line_ring_output_vsync();
}

int main(void)
{
    set_sys_clock_khz(126000, true);
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    // Heartbeat start
    for(int i=0; i<3; i++) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(50);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(50);
    }

    printf("\n\n=== SuperPico HDMI Mod ===\n");

    printf("Init video pipeline...\n");
    video_pipeline_init();

    printf("Init HDMI output...\n");
    hstx_di_queue_init();
    video_output_init(FRAME_WIDTH, FRAME_HEIGHT);
    video_output_set_scanline_callback(scanline_callback);
    video_output_set_vsync_callback(vsync_callback);

    printf("Launch Core 1 (HDMI)...\n");
    multicore_launch_core1(video_output_core1_run);
    
    sleep_ms(200);

    printf("Init video capture...\n");
    video_capture_init(SNES_V_ACTIVE);

    printf("Starting capture loop...\n");
    video_capture_run();

    return 0;
}
