#pragma once
#include <M5StickCPlus.h>
#include "Data.h"
#include "UIState.h"

struct RootSprite : public TFT_eSprite {
  RootSprite() : TFT_eSprite(&M5.Lcd) {}
};

class UI {
private:
  struct ColorScheme {
    uint16_t fg;
    uint16_t bg;
  };

  struct ColorSchemes {
    ColorScheme normal = { TFT_WHITE, TFT_BLACK };
    ColorScheme selected = { TFT_BLACK, TFT_LIGHTGREY };
    ColorScheme highlighted = { TFT_WHITE, TFT_RED };
  } colorSchemes;

  Data& data;
  UIState& uiState;

  RootSprite playSprite;
  RootSprite menuItemsSprite;
  RootSprite dataSprite;

  std::array<std::function<void()>, 4> dataRenderers {
    [this](){ drawIntegerData(data.controlChangeNumber); },
    [this](){ drawIntegerData(data.minValue); },
    [this](){ drawIntegerData(data.force); },
    [this](){ drawCurve(data.curve); },
  };

public:
  UI(Data& data, UIState& uiState) : data(data), uiState(uiState) {}

  void begin();
  void draw();

private:
  void drawMenuItems();
  void drawIntegerString(TFT_eSprite& s, int value, int x, int y);
  void drawIntegerData(int value);
  void drawCurve(float curve);

  void drawPlayScreen();
  void drawMenuScreen();
};