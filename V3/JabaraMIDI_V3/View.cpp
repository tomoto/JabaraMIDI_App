#include "View.h"

#include "images/images.h"

static void reverseBits(uint8_t* p, int len) {
  for (int i = 0; i < len; i++) {
    int b = p[i];
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    p[i] = b;
  }
}

View::View(M5GFX& gfx, ViewData& data, ViewState& state)
    : gfx(gfx),
      data(data),
      state(state),
      playSprite(&gfx),
      menuItemsSprite(&gfx),
      dataSprite(&gfx) {}

void View::begin() {
  gfx.setRotation(3);
  gfx.fillScreen(TFT_BLACK);

  menuItemsSprite.createSprite(gfx.width(), 30);
  dataSprite.createSprite(gfx.width(), 98);
  playSprite.createSprite(gfx.width(), gfx.height());

  reverseBits(muteImage, muteImageWidth * muteImageHeight / 8);
  reverseBits(musicImage, musicImageWidth * musicImageHeight / 8);
}

void View::draw() {
  if (state.mode == ViewState::Mode::PLAY) {
    drawPlayScreen();
  } else {
    drawMenuScreen();
  }
}

void View::drawMenuItems() {
  const char* labels[] = {"CC#", "CC Min", "Force", "Curve"};
  menuItemsSprite.setFont(&FreeSans12pt7b);

  ColorScheme cs = (state.mode == ViewState::Mode::MENU)
                       ? colorSchemes.highlighted
                       : colorSchemes.normal;

  menuItemsSprite.fillRect(0, 0, 100, 28, cs.bg);
  menuItemsSprite.setTextColor(cs.fg);
  menuItemsSprite.drawString(labels[state.menuItem], 4, 4);

  if (state.mode == ViewState::Mode::MENU) {
    int itemHeight = menuItemsSprite.height() / state.menuItemCount;
    for (int i = 0; i < state.menuItemCount; i++) {
      menuItemsSprite.fillRect(102, itemHeight * i, 26, itemHeight - 1,
                               (state.menuItem == i) ? cs.bg : TFT_DARKGRAY);
    }
  } else {
    menuItemsSprite.fillRect(102, 0, 26, menuItemsSprite.height(), TFT_BLACK);
  }
}

void View::drawIntegerString(LGFX_Sprite& s, int value, int x, int y) {
  char buf[20];
  sprintf(buf, "%d", value);
  s.drawString(buf, x, y);
}

void View::drawIntegerData(int value) {
  dataSprite.setTextFont(7);
  dataSprite.setTextColor(TFT_WHITE);
  dataSprite.setTextDatum(TR_DATUM);
  const int outerMargin = 16;
  const int innerMargin = 8;
  if (state.mode == ViewState::Mode::MENUITEM) {
    dataSprite.fillRect(0, outerMargin, dataSprite.width(),
                        dataSprite.height() - outerMargin * 2, TFT_RED);
    ;
  }
  drawIntegerString(dataSprite, value, dataSprite.width() - innerMargin,
                    outerMargin + innerMargin);
}

void View::drawCurve(float curve) {
  dataSprite.drawRect(
      0, 0, dataSprite.width(), dataSprite.height(),
      state.mode == ViewState::Mode::MENUITEM ? TFT_RED : TFT_WHITE);
  int innerWidth = dataSprite.width() - 2;
  int innerHeight = dataSprite.height() - 2;
  for (int i = 0; i < 4; i++) {
    int ii = i + 1;
    dataSprite.drawLine(1 + innerWidth * pow(i * 0.25, 1.0 / curve),
                        innerHeight - i * innerHeight / 4,
                        1 + innerWidth * pow(ii * 0.25, 1.0 / curve),
                        innerHeight - ii * innerHeight / 4, TFT_WHITE);
  }
}

void View::drawPlayScreen() {
  int c = playSprite.width() / 2;
  playSprite.fillScreen(TFT_BLACK);
  if (data.mute) {
    playSprite.drawXBitmap(c - muteImageWidth / 2, 40, muteImage,
                           muteImageWidth, muteImageHeight, TFT_YELLOW);
  } else {
    playSprite.drawXBitmap(c - musicImageWidth / 2, 40, musicImage,
                           musicImageWidth, musicImageHeight, TFT_GREEN);
  }

  playSprite.setTextDatum(TC_DATUM);
  playSprite.setTextColor(TFT_WHITE);
  playSprite.drawString("Click to Mute/Unmute", c, 90);
  playSprite.drawString("Long press for Menu", c, 100);

  playSprite.pushSprite(0, 0);
}

void View::drawMenuScreen() {
  menuItemsSprite.fillScreen(TFT_BLACK);
  dataSprite.fillScreen(TFT_BLACK);

  drawMenuItems();
  dataRenderers[state.menuItem]();

  menuItemsSprite.pushSprite(0, 0);
  dataSprite.pushSprite(0, 30);
}
