#include "Base.h"

#include "Graphics.h"

void halt(const char* message) {
  Serial.println(message);
  gfx.begin();
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextColor(TFT_ORANGE);
  gfx.setCursor(0, 0, 1);
  gfx.print(message);

  while (true) {
    delay(10);
  }
}
