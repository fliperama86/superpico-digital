#ifndef MELODIES_H
#define MELODIES_H

#include <stdint.h>

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
  D5S = 622, // Eb5
  E5 = 659,
  F5 = 698,
  G5 = 784,
  G5S = 831, // Ab5
  A5 = 880,
  Bb4 = 466,
  Bb5 = 932,
  C6 = 1046,
  D6 = 1175,
  Eb5 = 1245,
  F6 = 1397
};

static const note_t melody_korobeiniki[] = {
    {E5, 18}, {B4, 9},  {C5, 9},  {D5, 18},   {C5, 9},  {B4, 9},    {A4, 18},
    {A4, 9},  {C5, 9},  {E5, 18}, {D5, 9},    {C5, 9},  {B4, 27},   {C5, 9},
    {D5, 18}, {E5, 18}, {C5, 18}, {A4, 18},   {A4, 18}, {REST, 18}, {D5, 27},
    {F5, 9},  {A5, 18}, {G5, 9},  {F5, 9},    {E5, 27}, {C5, 9},    {E5, 18},
    {D5, 9},  {C5, 9},  {B4, 18}, {B4, 9},    {C5, 9},  {D5, 18},   {E5, 18},
    {C5, 18}, {A4, 18}, {A4, 18}, {REST, 18}, {E5, 36}, {C5, 36},   {D5, 36},
    {B4, 36}, {C5, 36}, {A4, 36}, {G4S, 36},  {B4, 36}, {E5, 36},   {C5, 36},
    {D5, 36}, {B4, 36}, {C5, 18}, {E5, 18},   {A5, 36}, {G5S, 72},
};

#endif // MELODIES_H
