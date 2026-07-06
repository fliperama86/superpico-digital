#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "tusb.h"

#include "pico_hdmi/video_output_rt.h"
#include "pico_hdmi/hstx_data_island_queue.h"

#include "video/video_pipeline.h"
#include "video/video_capture.h"
#include "video/freq_counter.h"
#include "video/snes_timing.h"
#include "video/video_config.h"
#include "config.h"
#include "settings.h"
#include "snes_pins.h"

#include "hardware/pio.h"

#if ENABLE_AUDIO
#include "audio/audio_pipeline.h"
#endif
#if ENABLE_OSD
#include "experiments/menu_diag_experiment.h"
#include "osd/fast_osd.h"
#endif

#include <stdbool.h>
#include <stdio.h>

#define SYS_CLK_60HZ_KHZ 126000U
#define SYS_CLK_480P_KHZ 252000U
#define SYS_CLK_720P_KHZ 372000U

static const video_mode_t *video_output_mode_for_reboot_mode(video_pipeline_reboot_mode_t mode)
{
#if ENABLE_REBOOT_MODE_SWITCH_720P
    if (mode == VIDEO_PIPELINE_REBOOT_MODE_720P) {
        return &video_mode_720_p;
    }
#else
    if (mode == VIDEO_PIPELINE_REBOOT_MODE_720P) {
        mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
    }
#endif
    return (mode == VIDEO_PIPELINE_REBOOT_MODE_240P) ? &video_mode_240_p : &video_mode_480_p;
}

static void configure_system_clock_for_mode(video_pipeline_reboot_mode_t mode)
{
    uint32_t sys_clk_khz = SYS_CLK_60HZ_KHZ;
    if (mode == VIDEO_PIPELINE_REBOOT_MODE_720P) {
        sys_clk_khz = SYS_CLK_720P_KHZ;
        vreg_set_voltage(VREG_VOLTAGE_1_30);
        sleep_ms(10);
    } else if (mode == VIDEO_PIPELINE_REBOOT_MODE_480P) {
        sys_clk_khz = SYS_CLK_480P_KHZ;
        vreg_set_voltage(VREG_VOLTAGE_1_30);
        sleep_ms(10);
    }
    set_sys_clock_khz(sys_clk_khz, true);
}

static void combined_background_task(void)
{
#if ENABLE_AUDIO
    audio_pipeline_process();
#endif
#if ENABLE_OSD
    menu_diag_experiment_tick_background();
#endif
}

int main(void)
{
    sleep_ms(1000);

#if ENABLE_REBOOT_MODE_SWITCH
    video_pipeline_reboot_mode_t boot_mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
    const bool warm_reboot = video_pipeline_take_reboot_mode_boot_request(&boot_mode);
#if ENABLE_OSD_RES_CONFIRM && ENABLE_OSD
    video_pipeline_reboot_mode_t previous_mode;
    if (video_pipeline_take_pending_confirmation(&previous_mode)) {
        menu_diag_experiment_arm_res_confirm(boot_mode, previous_mode);
    }
#endif
#if ENABLE_SETTINGS_FLASH
    if (!warm_reboot) {
        superpico_settings_t persisted;
        settings_load(&persisted);
        if (persisted.resolution <= (uint8_t)VIDEO_PIPELINE_REBOOT_MODE_720P) {
            boot_mode = (video_pipeline_reboot_mode_t)persisted.resolution;
#if !ENABLE_REBOOT_MODE_SWITCH_720P
            if (boot_mode == VIDEO_PIPELINE_REBOOT_MODE_720P) {
                boot_mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
            }
#endif
        }
    }
#endif
#else
    video_pipeline_reboot_mode_t boot_mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
#endif

    video_pipeline_set_reboot_requested_mode(boot_mode);
    configure_system_clock_for_mode(boot_mode);
    stdio_init_all();

    sleep_ms(500);
    stdio_flush();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

#if ENABLE_OSD
    gpio_init(PIN_OSD_BTN_MENU);
    gpio_set_dir(PIN_OSD_BTN_MENU, GPIO_IN);
    gpio_pull_up(PIN_OSD_BTN_MENU);

    gpio_init(PIN_OSD_BTN_BACK);
    gpio_set_dir(PIN_OSD_BTN_BACK, GPIO_IN);
    gpio_pull_up(PIN_OSD_BTN_BACK);
#endif

    printf("\n\n=== SuperPico HDMI Mod ===\n");

    printf("Init video pipeline...\n");

    // Match neopico-hd init order: DI queue → video pipeline → background task → capture → Core 1
    hstx_di_queue_init();
#if ENABLE_OSD
    fast_osd_init();
    menu_diag_experiment_init();
#endif
    video_pipeline_init();

    printf("Init HDMI output...\n");
    video_output_set_mode(video_output_mode_for_reboot_mode(boot_mode));
    video_output_init(FRAME_WIDTH, FRAME_HEIGHT);
    video_output_set_scanline_callback(scanline_callback);
    video_output_set_vsync_callback(vsync_callback);

#if ENABLE_AUDIO
    printf("Init audio pipeline...\n");
    audio_pipeline_init();
#endif
#if ENABLE_AUDIO || ENABLE_OSD
    video_output_set_background_task(combined_background_task);
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
