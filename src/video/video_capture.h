#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <stdint.h>

/**
 * Initialize SNES video capture hardware
 */
void video_capture_init(uint32_t height);

/**
 * Run video capture loop (never returns)
 */
void video_capture_run(void);

/**
 * Get current captured frame count
 */
uint32_t video_capture_get_frame_count(void);

#endif // VIDEO_CAPTURE_H
