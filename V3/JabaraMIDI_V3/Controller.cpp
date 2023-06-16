#include "Controller.h"

#include <Unit_Encoder.h>

#include "EncoderState.h"
#include "Engine.h"
#include "Graphics.h"
#include "Saver.h"
#include "View.h"
#include "ViewStateHandlers.h"

static ViewData viewData;
static ViewState viewState;
View ui(gfx, viewData, viewState);

Unit_Encoder encoder;
EncoderState encoderState;
Saver saver;

static void commitConfig(const ViewData& d) {
  JabaraMIDI::Config& t = Engine::config;
  t.midiControlNumber = d.controlChangeNumber;
  t.minValue = d.minValue;
  t.maxValue = 127;
  t.positiveScale = t.negativeScale = d.force;
  t.curveFactor = d.curve;
  t.mute = viewData.mute;
}

ViewStateHandlers handlers(viewData, viewState, saver, commitConfig);

void Controller::begin() {
  gfx.begin();

  encoder.begin();
  encoderState.lastValue = encoder.getEncoderValue();

  saver.begin();
  saver.load(viewData);
  commitConfig(viewData);

  ui.begin();
  ui.draw();
}

void Controller::process() {
  bool redrawScreen = false;

  int16_t encoderValue = encoder.getEncoderValue();

  // the encoder value increments 2 by 1 click...
  int16_t delta = (encoderValue - encoderState.lastValue) / 2;

  ViewStateHandler& h = handlers.getHandler();
  if (delta) {
    h.onChange(delta);
    encoderState.lastValue = encoderValue;
    redrawScreen = true;
  }

  bool buttonStatus = !encoder.getButtonStatus();  // inverted for some reason

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
}
