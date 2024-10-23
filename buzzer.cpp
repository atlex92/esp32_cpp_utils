#include "buzzer.hpp"
#include <algorithm>
#include "esp_log.h"
#include "string.h"

static const char* const TAG{"Buzzer"};
static const uint32_t kDefaultBuzzerPauseMs{1000U};

Buzzer::Buzzer(IBuzzerDriver* driver) : driver_(driver) {
  assert(driver_ != nullptr);
}

Buzzer::~Buzzer() {
  delete driver_;
}

void Buzzer::run(void* args) {
  driver_->initHardware();
  while (1) {
    if (!hasActiveMelody()) {
      loadNextMelodyFromQueue();
    }
    if (isCurrentMelodyFinished()) {
      mute();
      clearCurrentMelody();
      delay(kDefaultBuzzerPauseMs);
    } else {
      proccessMelody();
    }
  }
}

void Buzzer::waitForMelody() {
  if (false == melodies_.empty()) {
    return;
  }
  smphr_.take();
}

void Buzzer::mute() {
  driver_->off();
}

bool Buzzer::hasActiveMelody() const {
  return (current_melody_.size > 0U);
}

void Buzzer::proccessMelody() {
  if (!hasActiveMelody()) {
    return;
  }
  const size_t current_note{current_melody_.current_note};
  const uint32_t freq{
      getNoteFrequency(current_melody_.sounds[current_note].note, current_melody_.sounds[current_note].octave)};
  const uint32_t length_ms{current_melody_.sounds[current_note].length_ms};

  if (freq == 0U) {
    mute();
  } else {
    driver_->setFrequency(freq);
    driver_->on();
  }
  TickType_t start{xTaskGetTickCount()};
  xTaskDelayUntil(&start, pdMS_TO_TICKS(length_ms));
  ++current_melody_.current_note;
}

bool Buzzer::isCurrentMelodyFinished() const {
  return (current_melody_.current_note >= current_melody_.size);
}

void Buzzer::clearCurrentMelody() {
  current_melody_ = {0};
}

void Buzzer::loadNextMelodyFromQueue() {
  waitForMelody();
  if (true == melodies_.empty()) {
    return;
  }

  if (true == melodies_.front().is_looped) {
    for (auto it = melodies_.begin() + 1; it != melodies_.end(); it++) {
      if (false == it->is_looped) {
        std::iter_swap(melodies_.begin(), it);
        break;
      }
    }
  }

  current_melody_ = melodies_.front();
  if (false == current_melody_.is_looped) {
    melodies_.erase(melodies_.begin());
  }
}

bool Buzzer::isEnabled() const {
  return is_enabled_;
}

void Buzzer::enable() {
  resume();
  is_enabled_ = true;
}

void Buzzer::disable() {
  mute();
  suspend();
  is_enabled_ = false;
}

void Buzzer::addMelodyToQueue(const Melody& melody) {
  if (false == is_enabled_) {
    return;
  }

  auto duplicate_melody_lambda =
      std::find_if(melodies_.begin(), melodies_.end(),
                   [&melody](const Melody& inner_melody) { return (strcmp(melody.tag, inner_melody.tag) == 0); });

  if (duplicate_melody_lambda == melodies_.end()) {
    melodies_.push_back(melody);
    smphr_.tryGive();
  }
}

void Buzzer::deleteMelodyFromQueue(const char* const tag) {
  auto remove_melodies_lambda = [&](const Melody& melody) -> bool { return (0 == strcmp(tag, melody.tag)); };
  auto remove_it{std::remove_if(melodies_.begin(), melodies_.end(), remove_melodies_lambda)};
  melodies_.erase(remove_it, melodies_.end());
}
