#pragma once

#include "Base.h"

struct ViewData {
  int controlChangeNumber = 7;
  int minValue = 48;
  int force = 500;
  float curve = 0.7;
  bool mute = false;
};
