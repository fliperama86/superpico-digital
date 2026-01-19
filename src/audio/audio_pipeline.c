#include "audio_pipeline.h"
#include "config.h"
#include "video/video_config.h"

#if ENABLE_AUDIO
#include "melodies.h"
#include "pico/stdlib.h"
#include "pico_hdmi/hstx_data_island_queue.h"
#include "pico_hdmi/hstx_packet.h"
#include <math.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE 48000
#define TONE_AMPLITUDE 6000
#define SINE_TABLE_SIZE 256

static int16_t sine_table[SINE_TABLE_SIZE];
static uint32_t audio_phase = 0;
static uint32_t phase_increment = 0;
static int audio_frame_counter = 0;

static const note_t *current_melody = melody_korobeiniki;
static uint32_t current_melody_len =
    sizeof(melody_korobeiniki) / sizeof(melody_korobeiniki[0]);

static int melody_index = 0;
static int note_frames_remaining = 0;

static void init_sine_table(void) {
  for (int i = 0; i < SINE_TABLE_SIZE; i++) {
    float angle = (float)i * 2.0F * 3.14159265F / SINE_TABLE_SIZE;
    sine_table[i] = (int16_t)(sinf(angle) * TONE_AMPLITUDE);
  }
}

static inline int16_t get_sine_sample(void) {
  if (phase_increment == 0)
    return 0;
  int16_t s = sine_table[(audio_phase >> 24) & 0xFF];
  audio_phase += phase_increment;
  return s;
}

void audio_pipeline_init(void) {
  init_sine_table();
  melody_index = 0;
  note_frames_remaining = current_melody[0].duration;
  uint16_t freq = current_melody[0].freq;
  phase_increment =
      (freq > 0) ? (uint32_t)(((uint64_t)freq << 32) / AUDIO_SAMPLE_RATE) : 0;
}

/**
 * audio_pipeline_process: Runs on Core 1 as a background task
 * Fills the HDMI data island queue when there's space.
 */
void __scratch_x("") audio_pipeline_process(void) {
  while (hstx_di_queue_get_level() < 128) {
    audio_sample_t samples[4];
    for (int i = 0; i < 4; i++) {
      int16_t s = get_sine_sample();
      samples[i].left = s;
      samples[i].right = s;
    }
    hstx_packet_t packet;
    audio_frame_counter =
        hstx_packet_set_audio_samples(&packet, samples, 4, audio_frame_counter);
    hstx_data_island_t island;
    hstx_encode_data_island(&island, &packet, false, true);
    hstx_di_queue_push(&island);
  }
}

void audio_pipeline_step(void) {
  if (--note_frames_remaining <= 0) {
    melody_index = (melody_index + 1) % current_melody_len;
    note_frames_remaining = current_melody[melody_index].duration;
    uint16_t freq = current_melody[melody_index].freq;
    phase_increment =
        (freq > 0) ? (uint32_t)(((uint64_t)freq << 32) / AUDIO_SAMPLE_RATE) : 0;
  }
}
#endif
