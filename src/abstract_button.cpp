#include "abstract_button.hpp"
#include "assert.h"
#include "esp_log.h"
#include "time_utils.hpp"

#define BUTTON_TASK_PERIOD_MS 50UL

AbstractButton::AbstractButton(const uint32_t debounce_time_ms)
: Task{"AbstractButton", configMINIMAL_STACK_SIZE * 2}, debounce_time_ms_(debounce_time_ms) {
}

bool AbstractButton::init() {
  bool ret{false};
  if (true == initProcedure()) {
    is_initialized_ = true;
    ret = true;
  }
  return ret;
}

void AbstractButton::run(void* args) {
  if (false == is_initialized_) {
    ESP_LOGE(TAG, "not initialized yet");
    stop();
  }

  while (1) {
    poll();
    delay(BUTTON_TASK_PERIOD_MS);
  }
}

void AbstractButton::poll() {
  const auto now{TimeUtils::nowMs()};
  const eButtonStates state{readState()};
  const bool was_state_changed{(state != current_state_) &&
                               TimeUtils::isPeriodPassedMs(last_time_changed_, debounce_time_ms_)};
  if (was_state_changed) {
    last_time_changed_ = now;
    current_state_ = state;
    switch (state) {
      case eButtonStates::BUTTON_PRESSED:
        if (nullptr != on_pressed_cb_) {
          on_pressed_cb_();
        }
        break;

      case eButtonStates::BUTTON_RELEASED:
        was_on_held_cb_called_ = false;
        if (nullptr != on_released_cb_) {
          on_released_cb_();
        }
        break;
      default:
        assert(false);
        break;
    }
  }
  if (isHeldForMs(held_period_ms_) && !was_on_held_cb_called_ && on_held_cb_) {
    on_held_cb_();
    was_on_held_cb_called_ = true;
  }
}

eButtonStates AbstractButton::buttonState() const {
  return current_state_;
}

bool AbstractButton::isPressed() const {
  return (eButtonStates::BUTTON_PRESSED == current_state_);
}

bool AbstractButton::isReleased() const {
  return (eButtonStates::BUTTON_RELEASED == current_state_);
}

bool AbstractButton::isHeldForMs(const uint32_t time_ms) const {
  return (isPressed() && isCurrentStateConstFor(time_ms));
}

bool AbstractButton::isReleasedForMs(const uint32_t time_ms) const {
  return (isReleased() && isCurrentStateConstFor(time_ms));
}

void AbstractButton::onPressed(ButtonCallback cb) {
  on_pressed_cb_ = cb;
}

void AbstractButton::onReleased(ButtonCallback cb) {
  on_released_cb_ = cb;
}

void AbstractButton::onHeldForMs(ButtonCallback cb, const uint32_t held_period_ms) {
  on_held_cb_ = cb;
  held_period_ms_ = held_period_ms;
}

bool AbstractButton::isCurrentStateConstFor(const uint32_t time_ms) const {
  return TimeUtils::isPeriodPassedMs(last_time_changed_, time_ms);
}