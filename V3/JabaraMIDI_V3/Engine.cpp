#include "Engine.h"

#include <JabaraMIDI_Basics.h>
#include <JabaraMIDI_Sensor_BMP3XX.h>
#include <JabaraMIDI_USBMIDIHelper.h>

const int SAMPLES_PER_SECOND = 80;
const int MEASURE_INTERVAL = 1000 / SAMPLES_PER_SECOND;

JabaraMIDI::Config Engine::config;
int Engine::lastValue;
uint64_t Engine::taskCount;

static JabaraMIDI::Sensor_BMP3XX inside(0x77, &Wire);
static JabaraMIDI::Sensor_BMP3XX outside(0x76, &Wire);
static JabaraMIDI::Sensors<decltype(inside), decltype(outside)> sensors(
    inside, outside);

static JabaraMIDI::Calibrator<SAMPLES_PER_SECOND> calibrator;

static JabaraMIDI::USBMIDIHelper usbMIDI("Jabara MIDI V3");
static JabaraMIDI::Processor processor(Engine::config, [](JabaraMIDI::Config& c,
                                                          int value) {
  usbMIDI->sendControlChange(c.midiControlNumber, value, c.midiChannel + 1);
  Engine::lastValue = value;
  // Serial.printf("cc=%d, value=%d, ch=%d\n", c.midiControlNumber, value,
  // c.midiChannel + 1);
});

static void procesTask(void*) {
  while (true) {
    Engine::taskCount++;
    auto startMillis = millis();

    float i = sensors.readPressureInside();
    float o = sensors.readPressureOutside();
    float d = i - o;
    calibrator.process(d);
    float b = calibrator.getBaseline();
    processor.process(d - b);

    int consumedMillis = millis() - startMillis;
    delay(max(MEASURE_INTERVAL - consumedMillis, 1));
  }
}

static TaskHandle_t processTaskHandle;

void Engine::begin() {
  if (!sensors.begin()) {
    halt("Sensor initialization failed.");
  }

  usbMIDI.begin();

  xTaskCreateUniversal(procesTask, "Engine::processTask", 8192, NULL, 2,
                       &processTaskHandle, APP_CPU_NUM);
}
