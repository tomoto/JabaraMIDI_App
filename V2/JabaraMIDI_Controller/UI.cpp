#include "UI.h"
#include "images.h"

static void reverseBits(uint8_t* p, int len) {
  for (int i = 0; i < len; i++) {
    int b = p[i];
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    p[i] = b;
  }
}

void UI::begin() {
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);

  menuItemsSprite.createSprite(98, 130);
  dataSprite.createSprite(140, 130);
  playSprite.createSprite(M5.Lcd.width(), M5.Lcd.height());

  reverseBits(muteImage, muteImageWidth * muteImageHeight / 8);
  reverseBits(musicImage, musicImageWidth * musicImageHeight / 8);
}

void UI::draw() {
  if (uiState.mode == UIState::Mode::PLAY) {
    drawPlayScreen();
  } else {
    drawMenuScreen();
  }
}

void UI::drawMenuItems() {
  const char* labels[] = { "CC#", "CC Min", "Force", "Curve" };
  menuItemsSprite.setFreeFont(&FreeSans12pt7b);

  for (int i = 0; i < 4; i++) {
    ColorScheme cs =
      i == uiState.menuItem ?
        uiState.mode == UIState::Mode::MENU ?
          colorSchemes.highlighted :
          colorSchemes.selected :
        colorSchemes.normal;
    menuItemsSprite.fillRect(0, i * 30, 94, 28, cs.bg);
    menuItemsSprite.setTextColor(cs.fg);
    menuItemsSprite.drawString(labels[i], 4, i * 30 + 4);
  }
}

void UI::drawIntegerString(TFT_eSprite& s, int value, int x, int y) {
  char buf[20];
  sprintf(buf, "%d", value);
  s.drawString(buf, x, y);
}

void UI::drawIntegerData(int value) {
  dataSprite.setTextFont(7);
  dataSprite.setTextColor(TFT_WHITE);
  dataSprite.setTextDatum(TR_DATUM);
  if (uiState.mode == UIState::Mode::MENUITEM) {
    dataSprite.fillRect(10, 30, 120, 60, TFT_RED);
  }
  drawIntegerString(dataSprite, value, 120, 38);
}

void UI::drawCurve(float curve) {
  dataSprite.drawRect(10, 0, 122, 122, uiState.mode == UIState::Mode::MENUITEM ? TFT_RED : TFT_WHITE);
  for (int i = 0; i < 4; i++) {
    int ii = i + 1;
    dataSprite.drawLine(
      11 + 120 * pow(i * 0.25, 1.0 / curve),
      121 - i * 30,
      11 + 120 * pow(ii * 0.25, 1.0 / curve),
      121 - ii * 30,
      TFT_WHITE);
  }
}

void UI::drawPlayScreen() {
  int c = playSprite.width() / 2;
  playSprite.fillScreen(TFT_BLACK);
  if (data.mute) {
    playSprite.drawXBitmap(c - muteImageWidth / 2, 40, muteImage, muteImageWidth, muteImageHeight, TFT_YELLOW);
  } else {
    playSprite.drawXBitmap(c - musicImageWidth / 2, 40, musicImage, musicImageWidth, musicImageHeight, TFT_GREEN);
  }

  playSprite.setTextDatum(TC_DATUM);
  playSprite.setTextColor(TFT_WHITE);
  playSprite.drawString("Click to Mute/Unmute", c , 90);
  playSprite.drawString("Long press to switch to/from Menu", c , 100);

  playSprite.pushSprite(0, 0);
}

void UI::drawMenuScreen() {
  menuItemsSprite.fillScreen(TFT_BLACK);
  dataSprite.fillScreen(TFT_BLACK);

  drawMenuItems();
  dataRenderers[uiState.menuItem]();

  menuItemsSprite.pushSprite(2, 6);
  dataSprite.pushSprite(100, 6);
}
