#pragma once

#include <JabaraMIDI_Basics.h>

#include "Base.h"

class Engine {
 public:
  static JabaraMIDI::Config config;
  static int lastValue;
  static uint64_t taskCount;

  static void begin();
};
