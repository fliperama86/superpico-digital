#ifndef OSD_H
#define OSD_H

#include <stdbool.h>
#include <stdint.h>

// OSD box dimensions (in 640x480 space)
#define OSD_BOX_X 40        // Start X position (centered: (320-240)/2)
#define OSD_BOX_Y 80        // Start Y position (centered: (240-80)/2)
#define OSD_BOX_W 240       // Width in pixels (must be multiple of 8)
#define OSD_BOX_H 80        // Height in pixels

// Colors (RGB565)
#define OSD_COLOR_BG    0x0000  // Black background
#define OSD_COLOR_FG    0xFFFF  // White text
#define OSD_COLOR_TITLE 0x07FF  // Cyan for title

// OSD state
extern volatile bool osd_visible;

// Pre-rendered RGB565 buffer for the OSD box
extern uint16_t osd_framebuffer[OSD_BOX_H][OSD_BOX_W];

// Initialize OSD system
void osd_init(void);

// Clear OSD to background color
void osd_clear(void);

// Draw a character at (x,y) relative to OSD box origin
void osd_putchar(int x, int y, char c, uint16_t color);

// Draw a string at (x,y) relative to OSD box origin
void osd_puts(int x, int y, const char *str, uint16_t color);

// Show/hide OSD
static inline void osd_show(void) { osd_visible = true; }
static inline void osd_hide(void) { osd_visible = false; }
static inline void osd_toggle(void) { osd_visible = !osd_visible; }

#endif // OSD_H
