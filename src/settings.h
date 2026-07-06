#ifndef SUPERPICO_SETTINGS_H
#define SUPERPICO_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

// Flash-backed persistent settings. Stored in the last 4 KB flash sector as a
// magic+version+CRC record and written only during resolution-change reboot.
typedef struct {
    uint8_t resolution;   // video_pipeline_reboot_mode_t: 0=480p, 1=240p, 2=720p
    uint8_t reserved[31]; // future settings
} superpico_settings_t;

bool settings_load(superpico_settings_t *out);
void settings_save(const superpico_settings_t *s);

#endif // SUPERPICO_SETTINGS_H
