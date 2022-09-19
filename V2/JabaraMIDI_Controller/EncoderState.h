#pragma once
#include <Arduino.h>

class EncoderState {
private:
  uint32_t buttonPressStartMillis = 0;
  bool isButtonPressConsumed = false;
  const uint32_t longPressThresholdMillis = 1000;

public:
  int16_t lastValue = 0;

  enum ButtonAction {
    NONE, CLICKED, LONG_PRESSED
  };

  ButtonAction updateButtonStatus(bool newButtonStatus) {
    if (!buttonPressStartMillis) {
      if (newButtonStatus) { // released -> pressed
        buttonPressStartMillis = millis();
        isButtonPressConsumed = false;
      }
    } else {
      if (!newButtonStatus) { // pressed -> released
        buttonPressStartMillis = 0;
        if (!isButtonPressConsumed) {
          return ButtonAction::CLICKED;
        }
      } else { // pressed -> pressed
        if (!isButtonPressConsumed && millis() - buttonPressStartMillis >= longPressThresholdMillis) {
          isButtonPressConsumed = true;
          return ButtonAction::LONG_PRESSED;
        }
      }
    }
    
    return ButtonAction::NONE;
  }
};
