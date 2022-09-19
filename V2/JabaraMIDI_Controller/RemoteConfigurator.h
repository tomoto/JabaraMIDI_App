#pragma once
#include "Data.h"
#include "JabaraMIDI_RemoteConfigurator.h"

class RemoteConfigurator {
private:
  Data& data;
  HardwareSerial& stream;
  JabaraMIDI::Config jmConfig;
  JabaraMIDI::RemoteConfigurator<HardwareSerial> jmConfigurator;

public:
  RemoteConfigurator(Data& data, HardwareSerial& stream) :
    data(data), stream(stream), jmConfigurator(jmConfig, stream) {}

  void send() {
    prepare();
    jmConfigurator.send();
  }

  void listen() {
    prepare();
    jmConfigurator.listen();
  }

private:
  void prepare() {
    jmConfig.midiControlNumber = data.controlChangeNumber;
    jmConfig.minValue = data.minValue;
    jmConfig.maxValue = 127;
    jmConfig.positiveScale = jmConfig.negativeScale = data.force;
    jmConfig.curveFactor = data.curve;
    jmConfig.mute = data.mute;
  }
};