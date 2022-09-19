#include <JabaraMIDI_Basics.h>
#include <JabaraMIDI_Sensor_BME280.h>
#include <JabaraMIDI_USBMIDIHelper.h>
#include <JabaraMIDI_RemoteConfigurator.h>

const int SDA_PIN = 10;
const int SCL_PIN = 11;
const int LED_PIN = 15;
const int SERIAL1_RX_PIN = 8;
const int SERIAL1_TX_PIN = 9;

const int SAMPLES_PER_SECOND = 100;
const int MEASURE_INTERVAL = 1000 / SAMPLES_PER_SECOND;

JabaraMIDI::Sensor_BME280 inside(0x77);
JabaraMIDI::Sensor_BME280 outside(0x76);
JabaraMIDI::Sensors<decltype(inside), decltype(outside)> sensors(inside, outside);

JabaraMIDI::Calibrator<SAMPLES_PER_SECOND> calibrator;

JabaraMIDI::Config config;
JabaraMIDI::USBMIDIHelper usbMIDI("Jabara MIDI");
JabaraMIDI::Processor processor(config, [](JabaraMIDI::Config& config, int value) {
  usbMIDI->sendControlChange(config.midiControlNumber, value, config.midiChannel + 1);
  digitalWrite(LED_PIN, HIGH);
});

JabaraMIDI::RemoteConfigurator<decltype(Serial1)> remoteConfigurator(config, Serial1);

static void slowBlink() {
  digitalWrite(LED_PIN, millis() % 1000 < 500);
}

static void fastBlink() {
  digitalWrite(LED_PIN, (millis() % 250 < 100));
}

static void halt() {
  while (true) {
    fastBlink();
    delay(10);
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!sensors.begin()) {
    halt();
  }

  usbMIDI.begin();

  remoteConfigurator.request();
}

void loop() {
  remoteConfigurator.listen();

  float i = sensors.readPressureInside();
  float o = sensors.readPressureOutside();
  float d = i - o;
  calibrator.process(d);
  float b = calibrator.getBaseline();

  if (b == 0) { // not yet calibrated
    slowBlink();
  } else {
    digitalWrite(LED_PIN, LOW);
    processor.process(d - b);
  }

  delay(MEASURE_INTERVAL - millis() % MEASURE_INTERVAL);
}
