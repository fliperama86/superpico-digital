#ifndef VIDEO_PIPELINE_H
#define VIDEO_PIPELINE_H

#include <stdint.h>
#include "video_config.h"
#include "line_ring.h"

void video_pipeline_init(void);
void scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst);

#endif