#include "menu_diag_experiment.h"

#include "config.h"

#if ENABLE_OSD

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico_hdmi/hstx_data_island_queue.h"
#include "pico_hdmi/video_output_rt.h"

#if ENABLE_AUDIO
#include "audio/audio_pipeline.h"
#endif
#include "osd/fast_osd.h"
#include "osd/selftest_layout.h"
#include "settings.h"
#include "snes_pins.h"
#include "video/video_capture.h"
#include "video/video_pipeline.h"

extern volatile uint32_t video_frame_count;

#ifndef ENABLE_OSD_BOOT_OPEN
#define ENABLE_OSD_BOOT_OPEN 0
#endif
#ifndef ENABLE_REBOOT_MODE_SWITCH
#define ENABLE_REBOOT_MODE_SWITCH 0
#endif
#ifndef ENABLE_REBOOT_MODE_SWITCH_720P
#define ENABLE_REBOOT_MODE_SWITCH_720P 0
#endif
#ifndef ENABLE_OSD_RES_CONFIRM
#define ENABLE_OSD_RES_CONFIRM 0
#endif
#ifndef ENABLE_SETTINGS_FLASH
#define ENABLE_SETTINGS_FLASH 0
#endif

#define ROOT_TITLE_ROW 1
#define ROOT_FIRST_ENTRY_ROW 5
#define ROOT_IDLE_HIDE_MS 8000U
#define STATUS_UPDATE_FRAMES 30U
#define SELFTEST_UPDATE_FRAMES 60U
#define RES_CONFIRM_TIMEOUT_MS 10000U

#define BUTTON_DEBOUNCE_MS 200U

typedef enum {
    MENU_SCREEN_HIDDEN = 0,
    MENU_SCREEN_ROOT,
    MENU_SCREEN_RESOLUTION,
    MENU_SCREEN_STATUS,
    MENU_SCREEN_SELFTEST,
#if ENABLE_OSD_RES_CONFIRM
    MENU_SCREEN_RES_CONFIRM,
#endif
} menu_screen_t;

static menu_screen_t s_screen = MENU_SCREEN_HIDDEN;
static bool s_menu_was_pressed = false;
static bool s_back_was_pressed = false;
static uint32_t s_last_menu_press_ms = 0;
static uint32_t s_last_back_press_ms = 0;
static uint32_t s_last_input_ms = 0;
static uint8_t s_root_sel = 0;
static uint32_t s_last_status_frame = 0;
static uint32_t s_last_selftest_frame = 0;
static uint32_t s_video_hi = 0;
static uint32_t s_video_lo = 0;
static uint32_t s_video_samples = 0;
static uint32_t s_audio_hi = 0;
static uint32_t s_audio_lo = 0;
static uint32_t s_audio_samples = 0;
#if ENABLE_REBOOT_MODE_SWITCH
static video_pipeline_reboot_mode_t s_selected_mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
#endif
#if ENABLE_REBOOT_MODE_SWITCH && ENABLE_OSD_RES_CONFIRM
static bool s_res_confirm_armed = false;
static video_pipeline_reboot_mode_t s_res_confirm_new = VIDEO_PIPELINE_REBOOT_MODE_480P;
static video_pipeline_reboot_mode_t s_res_confirm_prev = VIDEO_PIPELINE_REBOOT_MODE_480P;
static uint32_t s_res_confirm_deadline_ms = 0;
static int32_t s_res_confirm_last_secs = -1;
#endif

static const char *const s_root_entry_labels[] = {
    "Resolution",
    "Status",
    "Self Test",
};
#define ROOT_ENTRY_COUNT ((uint8_t)(sizeof(s_root_entry_labels) / sizeof(s_root_entry_labels[0])))

static void root_menu_enter(uint32_t now_ms);

#if ENABLE_REBOOT_MODE_SWITCH
static const char *resolution_label(video_pipeline_reboot_mode_t mode)
{
    switch (mode) {
        case VIDEO_PIPELINE_REBOOT_MODE_240P:
            return "240p";
        case VIDEO_PIPELINE_REBOOT_MODE_720P:
            return "720p";
        default:
            return "480p";
    }
}

static const char *resolution_description(video_pipeline_reboot_mode_t mode)
{
    switch (mode) {
        case VIDEO_PIPELINE_REBOOT_MODE_240P:
            return "Direct Mode";
        case VIDEO_PIPELINE_REBOOT_MODE_720P:
            return "Experimental 3x";
        default:
            return "2x Integer Scaling";
    }
}

static video_pipeline_reboot_mode_t resolution_next(video_pipeline_reboot_mode_t mode)
{
    switch (mode) {
        case VIDEO_PIPELINE_REBOOT_MODE_240P:
            return VIDEO_PIPELINE_REBOOT_MODE_480P;
#if ENABLE_REBOOT_MODE_SWITCH_720P
        case VIDEO_PIPELINE_REBOOT_MODE_480P:
            return VIDEO_PIPELINE_REBOOT_MODE_720P;
#endif
        default:
            return VIDEO_PIPELINE_REBOOT_MODE_240P;
    }
}

static bool resolution_option_row(video_pipeline_reboot_mode_t mode, uint8_t *row)
{
    switch (mode) {
        case VIDEO_PIPELINE_REBOOT_MODE_240P:
            *row = 7;
            return true;
        case VIDEO_PIPELINE_REBOOT_MODE_480P:
            *row = 9;
            return true;
#if ENABLE_REBOOT_MODE_SWITCH_720P
        case VIDEO_PIPELINE_REBOOT_MODE_720P:
            *row = 11;
            return true;
#endif
        default:
            return false;
    }
}

static void resolution_render_description(void)
{
    fast_osd_puts_color(13, 2, "                    ", OSD_COLOR_GRAY);
    fast_osd_puts_color(13, 2, resolution_description(s_selected_mode), OSD_COLOR_GRAY);
}

static void resolution_render_option(uint8_t row, video_pipeline_reboot_mode_t mode)
{
    const bool selected = (s_selected_mode == mode);
    const bool current = (video_pipeline_reboot_requested_mode() == mode);
    const uint16_t color = selected ? OSD_COLOR_YELLOW : current ? OSD_COLOR_GREEN : OSD_COLOR_FG;
    const char *label = resolution_label(mode);
    fast_osd_putc_color(row, 3, selected ? '>' : ' ', color);
    fast_osd_puts_color(row, 5, label, color);
    if (current) {
        fast_osd_putc_color(row, (uint8_t)(5 + strlen(label)), '*', color);
    } else {
        fast_osd_putc_color(row, (uint8_t)(5 + strlen(label)), ' ', color);
    }
}

static void resolution_render_option_mode(video_pipeline_reboot_mode_t mode)
{
    uint8_t row = 0;
    if (resolution_option_row(mode, &row)) {
        resolution_render_option(row, mode);
    }
}

static void resolution_update_selection(video_pipeline_reboot_mode_t previous_mode)
{
    if (previous_mode == s_selected_mode) {
        return;
    }
    resolution_render_option_mode(previous_mode);
    resolution_render_option_mode(s_selected_mode);
    resolution_render_description();
}

static void resolution_draw(void)
{
    fast_osd_clear();
    fast_osd_puts_color(1, 2, "SuperPico Output", OSD_COLOR_YELLOW);
    fast_osd_puts_color(5, 2, "Resolution", OSD_COLOR_FG);
    resolution_render_option(7, VIDEO_PIPELINE_REBOOT_MODE_240P);
    resolution_render_option(9, VIDEO_PIPELINE_REBOOT_MODE_480P);
#if ENABLE_REBOOT_MODE_SWITCH_720P
    resolution_render_option(11, VIDEO_PIPELINE_REBOOT_MODE_720P);
#endif
    resolution_render_description();
}

static void resolution_enter(void)
{
    s_selected_mode = video_pipeline_reboot_requested_mode();
    resolution_draw();
    s_screen = MENU_SCREEN_RESOLUTION;
    osd_show();
}

static void resolution_apply(void)
{
    if (s_selected_mode == video_pipeline_reboot_requested_mode()) {
        root_menu_enter(to_ms_since_boot(get_absolute_time()));
        return;
    }

    osd_hide();
    s_screen = MENU_SCREEN_HIDDEN;
#if ENABLE_SETTINGS_FLASH
    superpico_settings_t persisted;
    settings_load(&persisted);
    persisted.resolution = (uint8_t)s_selected_mode;
    settings_save(&persisted);
#endif
#if ENABLE_OSD_RES_CONFIRM
    video_pipeline_request_reboot_mode_pending(s_selected_mode, video_pipeline_reboot_requested_mode());
#else
    video_pipeline_request_reboot_mode(s_selected_mode);
#endif
}

#if ENABLE_OSD_RES_CONFIRM
static void res_confirm_render_static(void)
{
    fast_osd_clear();
    fast_osd_puts_color(1, 2, "Keep this resolution?", OSD_COLOR_YELLOW);
    fast_osd_puts_color(4, 4, resolution_label(s_res_confirm_new), OSD_COLOR_GREEN);
    fast_osd_puts_color(7, 2, "Reverting in   s", OSD_COLOR_FG);
    fast_osd_puts_color(13, 2, "MENU keep BACK revert", OSD_COLOR_GRAY);
}

static void res_confirm_render_secs(int32_t secs)
{
    const uint8_t col = 15;
    fast_osd_putc_color(7, col, (secs >= 10) ? (char)('0' + (secs / 10)) : ' ', OSD_COLOR_FG);
    fast_osd_putc_color(7, (uint8_t)(col + 1), (char)('0' + (secs % 10)), OSD_COLOR_FG);
}

static void res_confirm_enter(uint32_t now_ms)
{
    s_screen = MENU_SCREEN_RES_CONFIRM;
    s_res_confirm_deadline_ms = now_ms + RES_CONFIRM_TIMEOUT_MS;
    s_res_confirm_last_secs = -1;
    res_confirm_render_static();
    osd_show();
}

static void res_confirm_keep(void)
{
    s_res_confirm_armed = false;
    osd_hide();
    s_screen = MENU_SCREEN_HIDDEN;
}

static void res_confirm_revert(void)
{
#if ENABLE_SETTINGS_FLASH
    superpico_settings_t persisted;
    settings_load(&persisted);
    persisted.resolution = (uint8_t)s_res_confirm_prev;
    settings_save(&persisted);
#endif
    video_pipeline_request_reboot_mode(s_res_confirm_prev);
}

void menu_diag_experiment_arm_res_confirm(video_pipeline_reboot_mode_t new_mode,
                                          video_pipeline_reboot_mode_t previous_mode)
{
    s_res_confirm_armed = true;
    s_res_confirm_new = new_mode;
    s_res_confirm_prev = previous_mode;
}
#endif
#endif

static void root_menu_render_entry(uint8_t idx)
{
    const bool selected = (s_root_sel == idx);
    const uint8_t row = (uint8_t)(ROOT_FIRST_ENTRY_ROW + (2U * idx));
    const uint16_t color = selected ? OSD_COLOR_YELLOW : OSD_COLOR_FG;
    fast_osd_putc_color(row, 3, selected ? '>' : ' ', color);
    fast_osd_puts_color(row, 5, s_root_entry_labels[idx], color);
}

static void root_menu_draw(void)
{
    fast_osd_clear();
    fast_osd_puts_color(ROOT_TITLE_ROW, 2, "SuperPico", OSD_COLOR_YELLOW);
    for (uint8_t i = 0; i < ROOT_ENTRY_COUNT; i++) {
        root_menu_render_entry(i);
    }
    fast_osd_puts_color(13, 2, "MENU enter BACK cycle", OSD_COLOR_GRAY);
}

static void root_menu_enter(uint32_t now_ms)
{
    root_menu_draw();
    s_screen = MENU_SCREEN_ROOT;
    s_last_input_ms = now_ms;
    osd_show();
}

static void status_draw_static(void)
{
    fast_osd_clear();
    fast_osd_puts_color(1, 2, "SuperPico Status", OSD_COLOR_YELLOW);
    fast_osd_puts_color(4, 2, "IN", OSD_COLOR_GRAY);
    fast_osd_puts_color(5, 2, "OUT", OSD_COLOR_GRAY);
    fast_osd_puts_color(6, 2, "DIQ", OSD_COLOR_GRAY);
#if ENABLE_AUDIO
    fast_osd_puts_color(8, 2, "AUD", OSD_COLOR_GRAY);
    fast_osd_puts_color(9, 2, "RATE", OSD_COLOR_GRAY);
    fast_osd_puts_color(10, 2, "OVF", OSD_COLOR_GRAY);
    fast_osd_puts_color(11, 2, "REARM", OSD_COLOR_GRAY);
#endif
    fast_osd_puts_color(14, 2, "MENU back", OSD_COLOR_GRAY);
}

static void put_u32(uint8_t row, uint8_t col, uint32_t value, uint16_t color)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%10lu", (unsigned long)value);
    fast_osd_puts_color(row, col, buf, color);
}

static void status_update_values(void)
{
    put_u32(4, 8, video_capture_get_frame_count(), OSD_COLOR_GREEN);
    put_u32(5, 8, video_frame_count, OSD_COLOR_GREEN);
    put_u32(6, 8, hstx_di_queue_get_level(), OSD_COLOR_GREEN);
#if ENABLE_AUDIO
    audio_pipeline_diag_t diag;
    audio_pipeline_get_diag(&diag);
    fast_osd_puts_color(8, 8, diag.muted ? "MUTED   " : "RUNNING ", diag.muted ? OSD_COLOR_YELLOW : OSD_COLOR_GREEN);
    put_u32(9, 8, diag.measured_rate_hz, diag.measured_rate_hz ? OSD_COLOR_GREEN : OSD_COLOR_YELLOW);
    put_u32(10, 8, diag.overflows, diag.overflows ? OSD_COLOR_YELLOW : OSD_COLOR_GREEN);
    put_u32(11, 8, diag.rearm_count, diag.rearm_count ? OSD_COLOR_YELLOW : OSD_COLOR_GREEN);
#endif
}

static void status_enter(void)
{
    status_draw_static();
    status_update_values();
    s_last_status_frame = video_frame_count;
    s_screen = MENU_SCREEN_STATUS;
    osd_show();
}

static void selftest_reset_counters(void)
{
    s_video_hi = 0;
    s_video_lo = 0;
    s_video_samples = 0;
    s_audio_hi = 0;
    s_audio_lo = 0;
    s_audio_samples = 0;
}

static void selftest_enter(void)
{
    selftest_layout_reset();
    selftest_reset_counters();
    s_last_selftest_frame = video_frame_count;
    s_screen = MENU_SCREEN_SELFTEST;
    osd_show();
}

static void root_menu_enter_leaf(void)
{
    if (s_root_sel == 0) {
        resolution_enter();
    } else if (s_root_sel == 1) {
        status_enter();
    } else {
        selftest_enter();
    }
}

static void sample_selftest_pins(void)
{
    uint32_t video_sample = 0;
    if (gpio_get(PIN_SNES_VBLANK)) video_sample |= SELFTEST_BIT_VBLANK;
    if (gpio_get(PIN_SNES_PCLK)) video_sample |= SELFTEST_BIT_PCLK;
    if (gpio_get(PIN_SNES_HBLANK)) video_sample |= SELFTEST_BIT_HBLANK;
    if (gpio_get(PIN_SNES_R0)) video_sample |= SELFTEST_BIT_R0;
    if (gpio_get(PIN_SNES_R1)) video_sample |= SELFTEST_BIT_R1;
    if (gpio_get(PIN_SNES_R2)) video_sample |= SELFTEST_BIT_R2;
    if (gpio_get(PIN_SNES_R3)) video_sample |= SELFTEST_BIT_R3;
    if (gpio_get(PIN_SNES_R4)) video_sample |= SELFTEST_BIT_R4;
    if (gpio_get(PIN_SNES_G0)) video_sample |= SELFTEST_BIT_G0;
    if (gpio_get(PIN_SNES_G1)) video_sample |= SELFTEST_BIT_G1;
    if (gpio_get(PIN_SNES_G2)) video_sample |= SELFTEST_BIT_G2;
    if (gpio_get(PIN_SNES_G3)) video_sample |= SELFTEST_BIT_G3;
    if (gpio_get(PIN_SNES_G4)) video_sample |= SELFTEST_BIT_G4;
    if (gpio_get(PIN_SNES_B0)) video_sample |= SELFTEST_BIT_B0;
    if (gpio_get(PIN_SNES_B1)) video_sample |= SELFTEST_BIT_B1;
    if (gpio_get(PIN_SNES_B2)) video_sample |= SELFTEST_BIT_B2;
    if (gpio_get(PIN_SNES_B3)) video_sample |= SELFTEST_BIT_B3;
    if (gpio_get(PIN_SNES_B4)) video_sample |= SELFTEST_BIT_B4;
    s_video_hi |= video_sample;
    s_video_lo |= ~video_sample;
    s_video_samples++;

    uint32_t audio_sample = 0;
    if (gpio_get(PIN_AUDIO_BCLK)) audio_sample |= SELFTEST_BIT_BCK;
    if (gpio_get(PIN_AUDIO_LRCK)) audio_sample |= SELFTEST_BIT_WS;
    if (gpio_get(PIN_AUDIO_SDATA)) audio_sample |= SELFTEST_BIT_DAT;
    if (gpio_get(PIN_AUDIO_DCK)) audio_sample |= SELFTEST_BIT_DCK;
    s_audio_hi |= audio_sample;
    s_audio_lo |= ~audio_sample;
    s_audio_samples++;
}

static void selftest_update_if_due(void)
{
    if ((video_frame_count - s_last_selftest_frame) < SELFTEST_UPDATE_FRAMES) {
        return;
    }
    s_last_selftest_frame = video_frame_count;

    uint32_t toggled_bits = 0;
    bool has_snapshot = false;
    if (s_video_samples > 0U) {
        toggled_bits |= s_video_hi & s_video_lo & SELFTEST_VIDEO_BITS_MASK;
        has_snapshot = true;
    }
    if (s_audio_samples > 0U) {
        toggled_bits |= s_audio_hi & s_audio_lo &
                        (SELFTEST_BIT_BCK | SELFTEST_BIT_WS | SELFTEST_BIT_DAT | SELFTEST_BIT_DCK);
        has_snapshot = true;
    }
    selftest_reset_counters();
    selftest_layout_update(video_frame_count, has_snapshot, toggled_bits);
}

void menu_diag_experiment_init(void)
{
    s_screen = MENU_SCREEN_HIDDEN;
    s_menu_was_pressed = false;
    s_back_was_pressed = false;
    s_last_menu_press_ms = 0;
    s_last_back_press_ms = 0;
    s_last_input_ms = to_ms_since_boot(get_absolute_time());
#if ENABLE_REBOOT_MODE_SWITCH
    s_selected_mode = video_pipeline_reboot_requested_mode();
#endif
    root_menu_draw();
    osd_hide();
#if ENABLE_REBOOT_MODE_SWITCH && ENABLE_OSD_RES_CONFIRM
    if (s_res_confirm_armed) {
        res_confirm_enter(s_last_input_ms);
        return;
    }
#endif
#if ENABLE_OSD_BOOT_OPEN
    selftest_enter();
#endif
}

void menu_diag_experiment_on_menu_open(void)
{
    root_menu_enter(to_ms_since_boot(get_absolute_time()));
}

void menu_diag_experiment_on_menu_close(void)
{
}

void menu_diag_experiment_tick_background(void)
{
    const uint32_t now_ms = to_ms_since_boot(get_absolute_time());
    const bool menu_pressed = !gpio_get(PIN_OSD_BTN_MENU);
    const bool back_pressed = !gpio_get(PIN_OSD_BTN_BACK);
    const bool menu_edge = menu_pressed && !s_menu_was_pressed && (now_ms - s_last_menu_press_ms) >= BUTTON_DEBOUNCE_MS;
    const bool back_edge = back_pressed && !s_back_was_pressed && (now_ms - s_last_back_press_ms) >= BUTTON_DEBOUNCE_MS;

    if (menu_edge) s_last_menu_press_ms = now_ms;
    if (back_edge) s_last_back_press_ms = now_ms;
    s_menu_was_pressed = menu_pressed;
    s_back_was_pressed = back_pressed;

    switch (s_screen) {
        case MENU_SCREEN_HIDDEN:
            if (menu_edge) {
                s_screen = MENU_SCREEN_ROOT;
                s_last_input_ms = now_ms;
                osd_show();
            }
            break;

        case MENU_SCREEN_ROOT:
            if (back_edge) {
                const uint8_t prev = s_root_sel;
                s_root_sel = (uint8_t)((s_root_sel + 1U) % ROOT_ENTRY_COUNT);
                root_menu_render_entry(prev);
                root_menu_render_entry(s_root_sel);
                s_last_input_ms = now_ms;
            } else if (menu_edge) {
                root_menu_enter_leaf();
            } else if ((now_ms - s_last_input_ms) >= ROOT_IDLE_HIDE_MS) {
                osd_hide();
                s_screen = MENU_SCREEN_HIDDEN;
            }
            break;

        case MENU_SCREEN_RESOLUTION:
            if (back_edge) {
                const video_pipeline_reboot_mode_t previous_mode = s_selected_mode;
                s_selected_mode = resolution_next(s_selected_mode);
                resolution_update_selection(previous_mode);
            } else if (menu_edge) {
                resolution_apply();
            }
            break;

#if ENABLE_REBOOT_MODE_SWITCH && ENABLE_OSD_RES_CONFIRM
        case MENU_SCREEN_RES_CONFIRM:
            if (menu_edge) {
                res_confirm_keep();
            } else if (back_edge || (int32_t)(now_ms - s_res_confirm_deadline_ms) >= 0) {
                res_confirm_revert();
            } else {
                int32_t secs = (int32_t)((s_res_confirm_deadline_ms - now_ms + 999U) / 1000U);
                if (secs > 99) {
                    secs = 99;
                }
                if (secs != s_res_confirm_last_secs) {
                    s_res_confirm_last_secs = secs;
                    res_confirm_render_secs(secs);
                }
            }
            break;
#endif

        case MENU_SCREEN_STATUS:
            if (menu_edge) {
                root_menu_enter(now_ms);
            } else if ((video_frame_count - s_last_status_frame) >= STATUS_UPDATE_FRAMES) {
                s_last_status_frame = video_frame_count;
                status_update_values();
            }
            break;

        case MENU_SCREEN_SELFTEST:
            if (menu_edge) {
                root_menu_enter(now_ms);
            } else {
                sample_selftest_pins();
                selftest_update_if_due();
            }
            break;

        default:
            s_screen = MENU_SCREEN_HIDDEN;
            osd_hide();
            break;
    }
}

#else

void menu_diag_experiment_init(void) {}
void menu_diag_experiment_on_menu_open(void) {}
void menu_diag_experiment_on_menu_close(void) {}
void menu_diag_experiment_tick_background(void) {}

#endif // ENABLE_OSD
