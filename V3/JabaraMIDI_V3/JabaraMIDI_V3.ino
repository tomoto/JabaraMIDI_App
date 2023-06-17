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

class FrequencyReporter {
 private:
  int intervalMillis = 1000;
  uint64_t reportCycle;

 public:
  void report() {
    auto cycle = millis() / intervalMillis;
    if (cycle != reportCycle) {
      Serial.println(Engine::taskCount);
      reportCycle = cycle;
      Engine::taskCount = 0;
    }
  }
} reporter;

void loop() {
  Controller::process();
  // reporter.report();
  delay(50);
}
