

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_NeoPixel.h>

// -------------------- User configuration --------------------
// DHT22
static const uint8_t kDhtPin = 7;          // matches original wiring
static const uint8_t kDhtType = DHT22;     // sensor model

// NeoPixel strip
static const uint8_t kPixelPin = 6;        // same wiring as original code
static const uint16_t kPixelCount = 7;     // number of LEDs connected

// Logging cadence (milliseconds)
static const uint32_t kPeriodMs = 2000UL;  // every 2 seconds

// -------------------- Globals --------------------
DHT dht(kDhtPin, kDhtType);
Adafruit_INA219 ina219;                    // default I2C address 0x40
Adafruit_NeoPixel pixels(kPixelCount, kPixelPin, NEO_GRB + NEO_KHZ800);

// Internal state for timing
uint32_t last_tick = 0;
bool header_emitted = false;

// -------------------- Utility helpers --------------------
static void statusPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < kPixelCount; ++i) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

static void emitHeaderOnce() {
  if (header_emitted) return;
  Serial.println(F("time_ms,bus_V,shunt_mV,current_mA,power_mW,temperature_C,humidity_%"));
  header_emitted = true;
}

static void emitError(const __FlashStringHelper* msg) {
  Serial.print(F("ERROR,"));
  Serial.println(msg);
}

// -------------------- Arduino lifecycle --------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  Wire.begin();
  pixels.begin();
  statusPixels(0, 0, 0);

  if (!ina219.begin()) {
    emitError(F("INA219 not found"));
  }

  dht.begin();
  statusPixels(255, 255, 255);
  emitHeaderOnce();
  last_tick = millis();
}

void loop() {
  const uint32_t now = millis();
  if (now - last_tick < kPeriodMs) {
    return;
  }
  last_tick = now;

  float busV = ina219.getBusVoltage_V();
  float shunt_mV = ina219.getShuntVoltage_mV();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();

  float temperature_C = dht.readTemperature();
  float humidity_pct  = dht.readHumidity();

  bool dht_ok = !isnan(temperature_C) && !isnan(humidity_pct);

  if (!dht_ok) {
    Serial.print(now);
    Serial.print(',');
    Serial.print(busV, 3);
    Serial.print(',');
    Serial.print(shunt_mV, 3);
    Serial.print(',');
    Serial.print(current_mA, 3);
    Serial.print(',');
    Serial.print(power_mW, 3);
    Serial.print(',');
    Serial.print(F(""));
    Serial.print(',');
    Serial.println(F(""));
    emitError(F("DHT22 read failed"));
    statusPixels(255, 180, 0);
    return;
  }

  Serial.print(now);
  Serial.print(',');
  Serial.print(busV, 3);
  Serial.print(',');
  Serial.print(shunt_mV, 3);
  Serial.print(',');
  Serial.print(current_mA, 3);
  Serial.print(',');
  Serial.print(power_mW, 3);
  Serial.print(',');
  Serial.print(temperature_C, 1);
  Serial.print(',');
  Serial.println(humidity_pct, 1);

  statusPixels(255, 255, 255);
}
