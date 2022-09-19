#pragma once

struct UIState {
  enum Mode { PLAY, MENU, MENUITEM } mode = PLAY;
  int menuItem = 0;
  const int menuItemCount = 4;
};
