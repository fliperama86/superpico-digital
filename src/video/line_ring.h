#ifndef LINE_RING_H
#define LINE_RING_H

#include "hardware/sync.h"
#include "video_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 256 lines = 128KB. Full frame buffer for SNES.
// This is the most stable approach and fits easily in RP2350 RAM.
#define LINE_RING_SIZE 256
#define LINE_WIDTH VIDEO_WIDTH

typedef struct {
  uint16_t lines[LINE_RING_SIZE][LINE_WIDTH];
  volatile uint32_t write_idx;
  volatile uint32_t frame_base_idx;
  volatile uint32_t read_frame_start;
} line_ring_t;

extern line_ring_t g_line_ring;

static inline void line_ring_vsync(void) {
  g_line_ring.frame_base_idx = g_line_ring.write_idx;
  __dmb();
}

static inline uint16_t *line_ring_write_ptr(uint16_t line) {
  uint32_t idx = g_line_ring.frame_base_idx + line;
  return g_line_ring.lines[idx % LINE_RING_SIZE];
}

static inline void line_ring_commit(uint16_t total_lines) {
  __dmb();
  g_line_ring.write_idx = g_line_ring.frame_base_idx + total_lines;
}

static inline void line_ring_output_vsync(void) {
  g_line_ring.read_frame_start = g_line_ring.frame_base_idx;
  __dmb();
}

static inline bool line_ring_ready(uint16_t line) {
  uint32_t target_idx = g_line_ring.read_frame_start + line;
  uint32_t write_pos = g_line_ring.write_idx;
  if (target_idx >= write_pos)
    return false;
  // With 256 lines, we can hold the entire SNES frame easily.
  return true;
}

static inline const uint16_t *line_ring_read_ptr(uint16_t line) {
  uint32_t target_idx = g_line_ring.read_frame_start + line;
  return g_line_ring.lines[target_idx % LINE_RING_SIZE];
}

#endif
