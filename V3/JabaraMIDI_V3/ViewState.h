#pragma once

#include "Base.h"

struct ViewState {
  enum Mode { PLAY, MENU, MENUITEM } mode = PLAY;
  int menuItem = 0;
  const int menuItemCount = 4;
};
