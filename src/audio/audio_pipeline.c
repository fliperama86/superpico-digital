#include "audio_pipeline.h"
#include "config.h"
#include "video/video_config.h"

#if ENABLE_AUDIO
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

typedef struct {
  uint16_t freq;
  uint8_t duration;
} note_t;

// Note frequencies (Hz)
enum {
  REST = 0,
  E3 = 165,
  G3S = 208,
  A3 = 220,
  B3 = 247,
  C4 = 262,
  D4 = 294,
  E4 = 330,
  F4 = 349,
  G4 = 392,
  G4S = 415,
  A4 = 440,
  B4 = 494,
  C5 = 523,
  D5 = 587,
  D5S = 622,
  E5 = 659,
  F5 = 698,
  G5 = 784,
  G5S = 831,
  A5 = 880
};

static const note_t melody[] = {
    {E5, 18}, {B4, 9},  {C5, 9},  {D5, 18},   {C5, 9},  {B4, 9},    {A4, 18},
    {A4, 9},  {C5, 9},  {E5, 18}, {D5, 9},    {C5, 9},  {B4, 27},   {C5, 9},
    {D5, 18}, {E5, 18}, {C5, 18}, {A4, 18},   {A4, 18}, {REST, 18}, {D5, 27},
    {F5, 9},  {A5, 18}, {G5, 9},  {F5, 9},    {E5, 27}, {C5, 9},    {E5, 18},
    {D5, 9},  {C5, 9},  {B4, 18}, {B4, 9},    {C5, 9},  {D5, 18},   {E5, 18},
    {C5, 18}, {A4, 18}, {A4, 18}, {REST, 18}, {E5, 36}, {C5, 36},   {D5, 36},
    {B4, 36}, {C5, 36}, {A4, 36}, {G4S, 36},  {B4, 36}, {E5, 36},   {C5, 36},
    {D5, 36}, {B4, 36}, {C5, 18}, {E5, 18},   {A5, 36}, {G5S, 72},
};

#define MELODY_LENGTH (sizeof(melody) / sizeof(melody[0]))

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
  note_frames_remaining = melody[0].duration;
  uint16_t freq = melody[0].freq;
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
    melody_index = (melody_index + 1) % MELODY_LENGTH;
    note_frames_remaining = melody[melody_index].duration;
    uint16_t freq = melody[melody_index].freq;
    phase_increment =
        (freq > 0) ? (uint32_t)(((uint64_t)freq << 32) / AUDIO_SAMPLE_RATE) : 0;
  }
}
#endif
