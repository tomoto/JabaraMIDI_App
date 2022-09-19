#include <M5StickCPlus.h>
#include <UNIT_ENCODER.h>
#include "images.h"

#include "Data.h"
#include "Saver.h"
#include "UIState.h"
#include "EncoderState.h"
#include "UI.h"
#include "StateHandlers.h"
#include "RemoteConfigurator.h"

Unit_Encoder encoder;

Data data;
Saver saver;
EncoderState encoderState;
UIState uiState;
UI ui(data, uiState);
RemoteConfigurator remoteConfigurator(data, Serial1);
StateHandlers handlers(data, uiState, saver, remoteConfigurator);

const int SDA_PIN = 32;
const int SCL_PIN = 33;
const int SERIAL1_RX_PIN = 0;
const int SERIAL1_TX_PIN = 26;

void setup() {
  M5.begin();

  Wire.begin(SDA_PIN, SCL_PIN);
  encoder.begin();
  encoderState.lastValue = encoder.getEncoderValue();

  saver.begin();
  saver.load(data);

  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);

  ui.begin();
  ui.draw();
}

void loop() {
  bool redrawScreen = false;

  int16_t encoderValue = encoder.getEncoderValue();

  // the encoder value increments 2 by 1 click...
  int16_t delta = (encoderValue - encoderState.lastValue) / 2;

  StateHandler& h = handlers.getHandler();
  if (delta) {
    h.onChange(delta);
    encoderState.lastValue = encoderValue;
    redrawScreen = true;
  }

  bool buttonStatus = !encoder.getButtonStatus(); // inverted for some reason

  switch (encoderState.updateButtonStatus(buttonStatus)) {
  case EncoderState::ButtonAction::CLICKED:
    h.onClick();
    redrawScreen = true;
    break;
  case EncoderState::ButtonAction::LONG_PRESSED:
    h.onLongPress();
    redrawScreen = true;
    break;
  }

  if (redrawScreen) {
    ui.draw();
  }

  if (M5.Axp.GetBatCurrent() < -50) { // Power input disconnected    
    M5.Axp.PowerOff();
  }

  remoteConfigurator.listen();

  delay(20);
}
