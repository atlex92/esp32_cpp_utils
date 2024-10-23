#pragma once

#include <stdint.h>

namespace Sound {

struct SoundObject {
  uint32_t freq;
  uint16_t length_ms;
};

enum eBaseNotes : uint16_t {
  NOTE_MUTE = 0,
  NOTE_C = 2640,
  NOTE_Cd = 2797,
  NOTE_D = 2963,
  NOTE_Dd = 3140,
  NOTE_E = 3326,
  NOTE_F = 3524,
  NOTE_Fd = 3733,
  NOTE_G = 3956,
  NOTE_Gd = 4191,
  NOTE_A = 4440,
  NOTE_Ad = 4704,
  NOTE_B = 4984
};

enum eOctaves : uint16_t { OCTAVE_1 = 0, OCTAVE_2, OCTAVE_3, OCTAVE_4, OCTAVE_5 };

struct NoteSoundObject {
  eBaseNotes note;
  eOctaves octave;
  uint16_t length_ms;
};

inline uint32_t getNoteFrequency(const eBaseNotes note, const eOctaves octave) {
  uint32_t ret{note};
  for (int i{}; i < octave; i++) {
    ret *= 2;
  }
  ret /= 10;
  return ret;
}
}; // namespace Sound
