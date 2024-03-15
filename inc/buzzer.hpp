#pragma once 

#include <vector>
#include "binary_semaphore.hpp"
#include "driver/gpio.h"
#include "sound.hpp"
#include "task.hpp"
#include "i_buzzer_driver.hpp"

#pragma pack(1)
struct Melody {
    const Sound::NoteSoundObject* sounds;
    size_t size;
    size_t current_note;
    bool is_looped;
    const char* tag;
};

class Buzzer : public Task{    
public:
    explicit Buzzer(IBuzzerDriver* const driver);
    ~Buzzer();
    void addMelodyToQueue(const Melody& melody);
    void deleteMelodyFromQueue(const char* const tag);
    bool isEnabled() const;
    void enable();
    void disable();
private:
    void mute();
    void clearCurrentMelody();
    bool isCurrentMelodyFinished()const;
    void proccessMelody();
    bool hasActiveMelody() const;
    void run(void* args) override;
    void loadNextMelodyFromQueue();
    void waitForMelody();
private:
    Melody current_melody_{};
    std::vector<Melody> melodies_;
    bool is_enabled_{false};
    IBuzzerDriver* const driver_{nullptr};
    BinarySemaphore smphr_;
};