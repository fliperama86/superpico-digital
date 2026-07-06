#include "video_pipeline.h"
#include "config.h"
#include "snes_timing.h"
#include "pico_hdmi/video_output_rt.h"
#include "hardware/structs/watchdog.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include <string.h>

#if ENABLE_OSD
#include "osd/fast_osd.h"
#endif
#if ENABLE_AUDIO
#include "audio/audio_pipeline.h"
#endif

#define OVERSCAN_COLOR_RGB565    0x0000   // black border
#define NO_SIGNAL_COLOR_RGB565   0x7BEF   // mid gray (~50%) - no-signal indicator

#define SNES_CANVAS_WIDTH 320U
#define SNES_CANVAS_HEIGHT 240U
#define SNES_CANVAS_H_MARGIN ((SNES_CANVAS_WIDTH - SNES_H_ACTIVE) / 2U)

#if ENABLE_REBOOT_MODE_SWITCH
#define REBOOT_MODE_BOOT_MAGIC 0x53505253U
#define REBOOT_MODE_BOOT_CHECK_XOR 0xa5a55a5aU
#define REBOOT_PENDING_MAGIC 0x53505000U
#if ENABLE_REBOOT_MODE_SWITCH_720P
#define REBOOT_MODE_MAX VIDEO_PIPELINE_REBOOT_MODE_720P
#else
#define REBOOT_MODE_MAX VIDEO_PIPELINE_REBOOT_MODE_240P
#endif

static video_pipeline_reboot_mode_t s_reboot_requested_mode = VIDEO_PIPELINE_REBOOT_MODE_480P;

static inline uint32_t reboot_mode_boot_check(uint32_t mode)
{
    return REBOOT_MODE_BOOT_MAGIC ^ mode ^ REBOOT_MODE_BOOT_CHECK_XOR;
}
#endif

line_ring_t g_line_ring __attribute__((aligned(64)));

void video_pipeline_init(void) {
    memset(&g_line_ring, 0, sizeof(g_line_ring));
}

static inline void __scratch_y("")
fill_rgb565(uint32_t *dst, uint32_t words, uint16_t color) {
    const uint32_t packed = ((uint32_t)color << 16) | color;
    for (uint32_t i = 0; i < words; i++) dst[i] = packed;
}

// Fast 2x pixel doubling
static inline void __scratch_y("")
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

static inline void __scratch_y("")
triple_pixels_fast(uint32_t *dst, const uint16_t *src, int count) {
    const uint32_t *src32 = (const uint32_t *)src;
    int pairs = count / 2;

    for (int i = 0; i < pairs; i++) {
        uint32_t two = src32[i];
        uint32_t p0 = two & 0xFFFF;
        uint32_t p1 = two >> 16;
        dst[(i * 3) + 0] = p0 | (p0 << 16);
        dst[(i * 3) + 1] = p0 | (p1 << 16);
        dst[(i * 3) + 2] = p1 | (p1 << 16);
    }
}

static inline void __scratch_y("")
quadruple_pixels_fast(uint32_t *dst, const uint16_t *src, int count) {
    const uint32_t *src32 = (const uint32_t *)src;
    int pairs = count / 2;

    for (int i = 0; i < pairs; i++) {
        uint32_t two = src32[i];
        uint32_t p0 = two & 0xFFFF;
        uint32_t p1 = two >> 16;
        uint32_t d0 = p0 | (p0 << 16);
        uint32_t d1 = p1 | (p1 << 16);
        dst[(i * 4) + 0] = d0;
        dst[(i * 4) + 1] = d0;
        dst[(i * 4) + 2] = d1;
        dst[(i * 4) + 3] = d1;
    }
}

typedef void (*pixel_scale_fn_t)(uint32_t *dst, const uint16_t *src, int count);

#if ENABLE_OSD
static bool s_osd_visible_latched = false;

static inline bool __scratch_x("")
osd_line_active(uint32_t source_line) {
    return s_osd_visible_latched && source_line >= OSD_BOX_Y && source_line < (OSD_BOX_Y + OSD_BOX_H);
}

static inline void __scratch_x("")
draw_osd_line_scaled(uint32_t *dst, uint32_t source_line, uint32_t osd_x_words, pixel_scale_fn_t scale_pixels) {
    const uint16_t *osd_src = osd_framebuffer[source_line - OSD_BOX_Y];
    scale_pixels(dst + osd_x_words, osd_src, OSD_BOX_W);
}
#else
static inline bool __scratch_x("")
osd_line_active(uint32_t source_line) {
    (void)source_line;
    return false;
}
#endif

void __scratch_x("") scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst)
{
    (void)v_scanline;

    const bool mode_is_240p = (video_output_active_mode->v_active_lines == 240U);
    const bool mode_is_720p = (video_output_active_mode->v_active_lines == 720U);
    const uint32_t h_words = video_output_active_mode->h_active_pixels / 2U;
    const uint32_t h_scale = mode_is_720p ? 3U : mode_is_240p ? 4U : 2U;
    const pixel_scale_fn_t scale_pixels =
        mode_is_720p ? triple_pixels_fast : mode_is_240p ? quadruple_pixels_fast : double_pixels_fast;

    if (mode_is_720p && ((active_line % 3U) != 0U)) {
        return;
    }

    const uint32_t source_line = mode_is_720p ? (active_line / 3U) : mode_is_240p ? active_line : (active_line >> 1);
    const uint32_t canvas_words = (SNES_CANVAS_WIDTH * h_scale) / 2U;
    const uint32_t canvas_margin_words = (h_words > canvas_words) ? ((h_words - canvas_words) / 2U) : 0U;
    const uint32_t image_words = (SNES_H_ACTIVE * h_scale) / 2U;
    const uint32_t image_x_words = canvas_margin_words + ((SNES_CANVAS_H_MARGIN * h_scale) / 2U);
#if ENABLE_OSD
    const uint32_t osd_x_words = canvas_margin_words + (((uint32_t)OSD_BOX_X * h_scale) / 2U);
    const uint32_t osd_w_words = ((uint32_t)OSD_BOX_W * h_scale) / 2U;
#endif

    const bool osd_active = osd_line_active(source_line);

    uint16_t fallback_color = OVERSCAN_COLOR_RGB565;
    const uint16_t *src = NULL;
    const uint32_t snes_line_u32 = source_line - V_OFFSET;
    if (source_line < SNES_CANVAS_HEIGHT && snes_line_u32 < SNES_V_ACTIVE) {
        const uint16_t snes_line = (uint16_t)snes_line_u32;
        if (line_ring_ready(snes_line)) {
            src = line_ring_read_ptr(snes_line);
        } else {
            fallback_color = NO_SIGNAL_COLOR_RGB565;
        }
    }

#if ENABLE_OSD
    if (osd_active) {
        if (!src) {
            fill_rgb565(dst, osd_x_words, fallback_color);
            draw_osd_line_scaled(dst, source_line, osd_x_words, scale_pixels);
            fill_rgb565(dst + osd_x_words + osd_w_words, h_words - osd_x_words - osd_w_words, fallback_color);
            return;
        }

        fill_rgb565(dst, image_x_words, OVERSCAN_COLOR_RGB565);
        scale_pixels(dst + image_x_words, src, OSD_BOX_X - SNES_CANVAS_H_MARGIN);
        draw_osd_line_scaled(dst, source_line, osd_x_words, scale_pixels);
        scale_pixels(dst + osd_x_words + osd_w_words,
                     src + (OSD_BOX_X + OSD_BOX_W - SNES_CANVAS_H_MARGIN),
                     SNES_CANVAS_H_MARGIN + SNES_H_ACTIVE - OSD_BOX_X - OSD_BOX_W);
        fill_rgb565(dst + image_x_words + image_words, h_words - image_x_words - image_words, OVERSCAN_COLOR_RGB565);
        return;
    }
#endif

    if (!src) {
        fill_rgb565(dst, h_words, fallback_color);
        return;
    }

    fill_rgb565(dst, image_x_words, OVERSCAN_COLOR_RGB565);
    scale_pixels(dst + image_x_words, src, SNES_H_ACTIVE);
    fill_rgb565(dst + image_x_words + image_words, h_words - image_x_words - image_words, OVERSCAN_COLOR_RGB565);
}

void __scratch_x("") vsync_callback(void) {
    line_ring_output_vsync();
#if ENABLE_AUDIO
    audio_pipeline_step();
#endif
#if ENABLE_OSD
    s_osd_visible_latched = osd_visible;
#endif
}

#if ENABLE_REBOOT_MODE_SWITCH
void video_pipeline_request_reboot_mode(video_pipeline_reboot_mode_t mode)
{
    if (mode > REBOOT_MODE_MAX) {
        mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
    }
    s_reboot_requested_mode = mode;
    watchdog_hw->scratch[0] = REBOOT_MODE_BOOT_MAGIC;
    watchdog_hw->scratch[1] = (uint32_t)mode;
    watchdog_hw->scratch[2] = reboot_mode_boot_check((uint32_t)mode);
    watchdog_hw->scratch[3] = 0;
    __dmb();
    watchdog_reboot(0, 0, 10);
    while (true) {
        tight_loop_contents();
    }
}

void video_pipeline_request_reboot_mode_pending(video_pipeline_reboot_mode_t mode,
                                                video_pipeline_reboot_mode_t previous)
{
    if (mode > REBOOT_MODE_MAX) {
        mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
    }
    if (previous > REBOOT_MODE_MAX) {
        previous = VIDEO_PIPELINE_REBOOT_MODE_480P;
    }
    s_reboot_requested_mode = mode;
    watchdog_hw->scratch[0] = REBOOT_MODE_BOOT_MAGIC;
    watchdog_hw->scratch[1] = (uint32_t)mode;
    watchdog_hw->scratch[2] = reboot_mode_boot_check((uint32_t)mode);
    watchdog_hw->scratch[3] =
        REBOOT_PENDING_MAGIC | (((uint32_t)previous & 0xFU) << 4) | (((uint32_t)previous ^ 0xAU) & 0xFU);
    __dmb();
    watchdog_reboot(0, 0, 10);
    while (true) {
        tight_loop_contents();
    }
}

video_pipeline_reboot_mode_t video_pipeline_reboot_requested_mode(void)
{
    return s_reboot_requested_mode;
}

void video_pipeline_set_reboot_requested_mode(video_pipeline_reboot_mode_t mode)
{
    if (mode > REBOOT_MODE_MAX) {
        mode = VIDEO_PIPELINE_REBOOT_MODE_480P;
    }
    s_reboot_requested_mode = mode;
}

bool video_pipeline_take_reboot_mode_boot_request(video_pipeline_reboot_mode_t *mode_out)
{
    const uint32_t magic = watchdog_hw->scratch[0];
    const uint32_t mode = watchdog_hw->scratch[1];
    const uint32_t check = watchdog_hw->scratch[2];
    watchdog_hw->scratch[0] = 0;
    watchdog_hw->scratch[1] = 0;
    watchdog_hw->scratch[2] = 0;

    if ((magic != REBOOT_MODE_BOOT_MAGIC) || (mode > (uint32_t)REBOOT_MODE_MAX) ||
        (check != reboot_mode_boot_check(mode))) {
        return false;
    }
    if (mode_out) {
        *mode_out = (video_pipeline_reboot_mode_t)mode;
    }
    s_reboot_requested_mode = (video_pipeline_reboot_mode_t)mode;
    return true;
}

bool video_pipeline_take_pending_confirmation(video_pipeline_reboot_mode_t *previous_mode)
{
    const uint32_t packed = watchdog_hw->scratch[3];
    watchdog_hw->scratch[3] = 0;

    const uint32_t mode = (packed >> 4) & 0xFU;
    const uint32_t check = packed & 0xFU;
    if (((packed & 0xFFFFFF00U) != REBOOT_PENDING_MAGIC) || (mode > (uint32_t)REBOOT_MODE_MAX) ||
        (check != ((mode ^ 0xAU) & 0xFU))) {
        return false;
    }
    if (previous_mode) {
        *previous_mode = (video_pipeline_reboot_mode_t)mode;
    }
    return true;
}
#endif
