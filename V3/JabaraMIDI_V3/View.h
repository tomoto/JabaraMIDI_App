#pragma once
#include <M5GFX.h>

#include "ViewData.h"
#include "ViewState.h"

class View {
 private:
  struct ColorScheme {
    uint16_t fg;
    uint16_t bg;
  };

  struct ColorSchemes {
    ColorScheme normal = {TFT_WHITE, TFT_BLACK};
    ColorScheme selected = {TFT_BLACK, TFT_LIGHTGREY};
    ColorScheme highlighted = {TFT_WHITE, TFT_RED};
  } colorSchemes;

  M5GFX& gfx;
  ViewData& data;
  ViewState& state;

  LGFX_Sprite playSprite;
  LGFX_Sprite menuItemsSprite;
  LGFX_Sprite dataSprite;

  std::array<std::function<void()>, 4> dataRenderers{
      [this]() { drawIntegerData(data.controlChangeNumber); },
      [this]() { drawIntegerData(data.minValue); },
      [this]() { drawIntegerData(data.force); },
      [this]() { drawCurve(data.curve); },
  };

 public:
  View(M5GFX& gfx, ViewData& data, ViewState& state);
  void begin();
  void draw();
  void drawPowerBar(int value, int range);

 private:
  void drawMenuItems();
  void drawIntegerString(LGFX_Sprite& s, int value, int x, int y);
  void drawIntegerData(int value);
  void drawCurve(float curve);

  void drawPlayScreen();
  void drawMenuScreen();
};