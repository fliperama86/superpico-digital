#include "video_pipeline.h"
#include "snes_timing.h"
#include "pico_hdmi/video_output.h"
#include "pico/stdlib.h"
#include <string.h>

line_ring_t g_line_ring __attribute__((aligned(64)));

void video_pipeline_init(void) {
    memset(&g_line_ring, 0, sizeof(g_line_ring));
}

// Fast 2x pixel doubling
static inline void __scratch_x("")
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

void __scratch_x("") scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst)
{
    (void)v_scanline;

    // 2x vertical scaling: 240 source lines -> 480 output lines
    uint32_t source_line = active_line / 2;

    if (source_line >= FRAME_HEIGHT / 2) {
        memset(dst, 0, MODE_H_ACTIVE_PIXELS * 2);
        return;
    }

    // Centering if needed (SNES is 224 lines, we show 240)
    if (source_line < V_OFFSET || source_line >= V_OFFSET + SNES_V_ACTIVE) {
        memset(dst, 0, MODE_H_ACTIVE_PIXELS * 2);
        return;
    }

    uint16_t snes_line = source_line - V_OFFSET;

    if (!line_ring_ready(snes_line)) {
        // Show very dim gray if not ready to distinguish from no-signal
        uint32_t gray32 = 0x08410841; 
        for(int i=0; i<MODE_H_ACTIVE_PIXELS/2; i++) dst[i] = gray32;
        return;
    }

    const uint16_t *src = line_ring_read_ptr(snes_line);
    
    // Horizontal centering: (640 - (256*2)) / 2 = 64 pixels left/right
    // 64 pixels = 32 uint32 pairs
    memset(dst, 0, 64 * 2);
    double_pixels_fast(dst + 32, src, SNES_H_ACTIVE);
    memset(dst + 32 + SNES_H_ACTIVE, 0, 64 * 2);
}
