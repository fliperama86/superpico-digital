/**
 * superpico-digital - CSYNC Debug Build (No HDMI)
 */

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "tusb.h"

#include "video/video_capture.h"
#include "video/snes_timing.h"

#include <stdio.h>

int main(void)
{
    set_sys_clock_khz(126000, true);
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // Wait for USB connection (Blinks LED while waiting)
    while (!tud_cdc_connected()) {
        gpio_xor_mask(1ul << PICO_DEFAULT_LED_PIN);
        sleep_ms(100);
        tud_task();
    }
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    printf("\n\n=== CSYNC Debug Build [v2] ===\n");
    printf("System clock: %lu Hz\n", clock_get_hz(clk_sys));

    printf("Initializing Video Capture (PIO)...\n");
    video_capture_init(SNES_V_ACTIVE);
    printf("Video Capture initialized.\n");

    printf("Starting capture loop (CSYNC debug)...\n");
    video_capture_run();

    return 0;
}