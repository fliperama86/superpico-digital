#ifndef AUDIO_PIPELINE_H
#define AUDIO_PIPELINE_H

#include <stdint.h>

void audio_pipeline_init(void);
void audio_pipeline_process(void);
void audio_pipeline_step(void);

#endif
