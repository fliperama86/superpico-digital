#ifndef VIDEO_PIPELINE_H
#define VIDEO_PIPELINE_H

#include <stdbool.h>
#include <stdint.h>
#include "video_config.h"
#include "line_ring.h"
#include "config.h"

void video_pipeline_init(void);
void scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst);
void vsync_callback(void);

#if ENABLE_REBOOT_MODE_SWITCH
typedef enum {
    VIDEO_PIPELINE_REBOOT_MODE_480P = 0,
    VIDEO_PIPELINE_REBOOT_MODE_240P = 1,
    VIDEO_PIPELINE_REBOOT_MODE_720P = 2,
} video_pipeline_reboot_mode_t;

void video_pipeline_request_reboot_mode(video_pipeline_reboot_mode_t mode);
void video_pipeline_request_reboot_mode_pending(video_pipeline_reboot_mode_t mode,
                                                video_pipeline_reboot_mode_t previous);
video_pipeline_reboot_mode_t video_pipeline_reboot_requested_mode(void);
void video_pipeline_set_reboot_requested_mode(video_pipeline_reboot_mode_t mode);
bool video_pipeline_take_reboot_mode_boot_request(video_pipeline_reboot_mode_t *mode);
bool video_pipeline_take_pending_confirmation(video_pipeline_reboot_mode_t *previous_mode);
#endif

#endif
