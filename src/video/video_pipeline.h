#ifndef VIDEO_PIPELINE_H
#define VIDEO_PIPELINE_H

#include <stdint.h>
#include "video_config.h"

// Full framebuffer (320x240 RGB565)
extern uint16_t g_framebuffer[VIDEO_HEIGHT][VIDEO_WIDTH];

void video_pipeline_init(void);
void scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst);

#endif