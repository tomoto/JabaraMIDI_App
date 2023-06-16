#include <Wire.h>

#include "Base.h"
#include "Controller.h"
#include "Engine.h"

const int SDA_PIN = 2;
const int SCL_PIN = 1;
const int I2C_SPEED = 200000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN, I2C_SPEED);

  Controller::begin();
  Engine::begin();
}

void loop() {
  Controller::process();
  delay(50);
}
