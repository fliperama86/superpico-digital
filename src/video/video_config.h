#ifndef VIDEO_CONFIG_H
#define VIDEO_CONFIG_H

// SNES native resolution
#define VIDEO_WIDTH 256
#define VIDEO_HEIGHT 224

// Internal 320x240 canvas. The RT HDMI backend scales this to the selected
// output mode: 480p, 240p, or 720p.
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

// Centering: (480 - (224*2)) / 2 = 16 lines top/bottom
#define V_OFFSET 8

#endif
