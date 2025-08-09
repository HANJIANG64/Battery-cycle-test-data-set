#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_NeoPixel.h>

// DHT22 Settings
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// INA219 Settings
Adafruit_INA219 ina219;

// LED strip light settings
#define LED_PIN 6 // LED data pin
#define NUM_LEDS 7 // Number of LED lamp beads
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  // Initialise DHT22
  dht.begin();

  // Initialise INA219
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip!");
    while (1);
  }

  // Set up INA219 calibration
  ina219.setCalibration_16V_400mA();

  // Initialise LED strip
  strip.begin();
  strip.show(); // Turn off all LEDs

  // Print header
  Serial.println("Time(s),Voltage(V),Current(mA),Power(mW),Temperature(C),Humidity(%)");
}

void loop() {
  // Read INA219 data
  float voltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  float power = ina219.getPower_mW();

  // Read DHT22 data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // If reading fails, delay for 1 second and try again.
  if (isnan(temperature) || isnan(humidity)) {
    delay(1000);
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
  }

  // Output only valid data
  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print(millis() / 1000.0); // time measured in seconds
    Serial.print(",");
    Serial.print(voltage);
    Serial.print(",");
    Serial.print(current);
    Serial.print(",");
    Serial.print(power);
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.println(humidity);
  } else {
    Serial.println("ERROR: DHT22 sensor failed.");
  }

  // Light up the LED light strip in white.
  strip.fill(strip.Color(255, 255, 255)); // white
  strip.show();

  delay(2000); // Record data every 2 seconds
}
