#include <M5StickC.h>
#include <Adafruit_BME280.h>

const int SDA_HAT = 0;
const int SCL_HAT = 26;

Adafruit_BME280 bme;

static void checkResetButton() {
  if (M5.BtnA.pressedFor(2000)) {
    M5.Axp.PowerOff();
  }
  if (M5.BtnA.wasReleased()) {
    ESP.restart();
  }
}

static void halt() {
  while (true) {
    M5.update();
    checkResetButton();
    delay(100);
  }
}

static void beginBME280()
{
  Wire.begin(SDA_HAT, SCL_HAT);

  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("Failed to initialize the BME280 sensor.");
    halt();
  }
  Serial.println("BME280 initialized.");

  bme.setSampling(
    Adafruit_BME280::MODE_NORMAL,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_NONE,
    Adafruit_BME280::FILTER_X4,
    Adafruit_BME280::STANDBY_MS_0_5
    );
}

void setup()
{
  M5.begin();
  M5.Axp.ScreenBreath(8);
  M5.Lcd.fillScreen(TFT_RED);
  Serial.begin(115200);
  beginBME280();
  M5.Lcd.fillScreen(TFT_GREEN);
}

const int MEASURE_INTERVAL = 19;

void loop()
{
  M5.update();
  checkResetButton();

  Serial.println(bme.readPressure(), 6);
  delay(MEASURE_INTERVAL);
}
