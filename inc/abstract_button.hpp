#pragma once

#include "i_button.hpp"
#include "task.hpp"

class AbstractButton : public IButton, public Task {
public:
  explicit AbstractButton(const uint32_t debounce_time_ms = 10U);
  eButtonStates buttonState() const override final;
  bool isPressed() const override final;
  bool isReleased() const override final;
  bool isHeldForMs(const uint32_t time_ms) const override final;
  bool isReleasedForMs(const uint32_t time_ms) const override final;
  void onPressed(ButtonCallback cb) override final;
  void onReleased(ButtonCallback cb) override final;
  void onHeldForMs(ButtonCallback cb, const uint32_t held_period_ms) override final;
  bool init() override final;
  void poll();
  void run(void* args) override final;

protected:
  virtual eButtonStates readState() const = 0; // abstracts button state reading procedure
  virtual bool initProcedure() = 0;            // abstracts button init procedure
  bool isCurrentStateConstFor(const uint32_t time_ms) const;

protected:
  bool is_initialized_{};

private:
  static constexpr const char* const TAG{"AbstractButton"};
  const uint32_t debounce_time_ms_{};
  eButtonStates current_state_{BUTTON_UNKNOWN};
  uint32_t held_period_ms_{};
  uint64_t last_time_changed_{};
  ButtonCallback on_pressed_cb_{};
  ButtonCallback on_released_cb_{};
  ButtonCallback on_held_cb_{};
  bool was_on_held_cb_called_{false};
};