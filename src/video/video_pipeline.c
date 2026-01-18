#include "video_pipeline.h"
#include "pico_hdmi/video_output.h"
#include "osd/osd.h"
#include "pico/stdlib.h"
#include <string.h>

// Full framebuffer (320x240 RGB565)
// Aligned for potential DMA usage (though currently accessed via CPU in ISR)
uint16_t g_framebuffer[VIDEO_HEIGHT][VIDEO_WIDTH] __attribute__((aligned(64)));

void video_pipeline_init(void) {
    // Initialize framebuffer with a white grid on black background
    for (int y = 0; y < VIDEO_HEIGHT; y++) {
        for (int x = 0; x < VIDEO_WIDTH; x++) {
            uint16_t color = 0x0000; // Black

            // Draw grid lines every 32 pixels
            // Use (x % 32 == 0) || (y % 32 == 0) for grid
            // Also draw a border
            if (x == 0 || x == VIDEO_WIDTH - 1 || y == 0 || y == VIDEO_HEIGHT - 1 ||
                (x % 32) == 0 || (y % 32) == 0) {
                color = 0xFFFF; // White
            }

            g_framebuffer[y][x] = color;
        }
    }
}

/**
 * Fast 2x pixel doubling: reads 2 pixels, writes 2 doubled words
 * Processes 32-bits at a time for efficiency
 */
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

    // 2x Vertical Scaling: Every 240p line is shown twice to reach 480p
    // active_line is 0..479. source_line is 0..239.
    uint32_t source_line = active_line / 2;

    // Bounds checking
    if (source_line >= VIDEO_HEIGHT) {
        memset(dst, 0, FRAME_WIDTH * 2); // Black output
        return;
    }

    // Read directly from the pre-filled framebuffer
    const uint16_t *src = g_framebuffer[source_line];

    // Check if OSD is visible AND this line intersects OSD box
    // OSD coordinates are in 240p space (source_line), same as video
    if (osd_visible && source_line >= OSD_BOX_Y && source_line < OSD_BOX_Y + OSD_BOX_H) {
        // OSD line within the OSD box
        uint32_t osd_line = source_line - OSD_BOX_Y;
        const uint16_t *osd_src = osd_framebuffer[osd_line];

        // === Loop splitting: 3 regions, no per-pixel branching ===

        // Region 1: Before OSD box (0 to OSD_BOX_X)
        double_pixels_fast(dst, src, OSD_BOX_X);

        // Region 2: OSD box (OSD_BOX_X to OSD_BOX_X + OSD_BOX_W)
        // OSD overlay is 1:1, so we double it to match the output scale
        double_pixels_fast(dst + OSD_BOX_X, osd_src, OSD_BOX_W);

        // Region 3: After OSD box (OSD_BOX_X + OSD_BOX_W to VIDEO_WIDTH)
        double_pixels_fast(dst + OSD_BOX_X + OSD_BOX_W,
                          src + OSD_BOX_X + OSD_BOX_W,
                          VIDEO_WIDTH - OSD_BOX_X - OSD_BOX_W);
    } else {
        // Fast path: no OSD on this line, full video doubling
        double_pixels_fast(dst, src, VIDEO_WIDTH);
    }
}
