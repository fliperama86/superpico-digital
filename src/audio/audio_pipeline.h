#ifndef AUDIO_PIPELINE_H
#define AUDIO_PIPELINE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t samples_output;
    uint32_t measured_rate_hz;
    uint32_t overflows;
    uint32_t rearm_count;
    uint32_t reset_count;
    bool muted;
    bool running;
} audio_pipeline_diag_t;

void audio_pipeline_init(void);
void audio_pipeline_process(void);
void audio_pipeline_step(void);
void audio_pipeline_request_rearm(void);
void audio_pipeline_get_diag(audio_pipeline_diag_t *diag);


#endif
