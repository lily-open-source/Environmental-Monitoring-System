// Define your Blynk template information and authentication token
#define BLYNK_TEMPLATE_ID "template_id"
#define BLYNK_TEMPLATE_NAME "template_name"
#define BLYNK_AUTH_TOKEN "auth_token"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SHT2x.h"
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "your_ssid";
char pass[] = "ssid_password";

// Define the pin for MQ-137
const int mq137Pin = 34;

// Initialize SHT20 sensor
uint32_t start, stop;
SHT2x sht;

// Initialize the LCD, set the I2C address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Function to read MQ-137 sensor value
float readMQ137() {
  int analogValue = analogRead(mq137Pin);
  // Convert analog value to ppm (calibration needed)
  float ppm = (analogValue / 4095.0) * 100; // Simplified conversion
  return ppm;
}

// Function to calculate heat index
float calculateHeatIndex(float temperature, float humidity) {
  // Fahrenheit temperature and relative humidity
  float tempF = (temperature * 1.8) + 32; // Convert Celsius to Fahrenheit
  
  // Check if temperature is below 80°F or humidity is below 40%
  if (tempF < 80.0 || humidity < 40.0) {
    return tempF; // Heat index is the same as the temperature
  }

  // Use the NWS formula for heat index calculation
  // NWS link : https://www.weather.gov/ama/heatindex
  float hi = -42.379 +
             2.04901523 * tempF +
            10.14333127 * humidity +
            -0.22475541 * tempF * humidity +
            -0.00683783 * pow(tempF, 2) +
            -0.05481717 * pow(humidity, 2) +
             0.00122874 * pow(tempF, 2) * humidity +
             0.00085282 * tempF * pow(humidity, 2) +
            -0.00000199 * pow(tempF, 2) * pow(humidity, 2);

  return hi;
}

// Function to categorize heat index
String categorizeHeatIndex(float hi) {
  if (hi <= 80) {
    return "A"; //"safe";
  } else if (hi >= 80 && hi <= 90) {
    return "B"; //"caution";
  } else if (hi >= 90 && hi <= 103) {
    return "C"; //"E caution";
  } else if (hi >= 103 && hi <= 124) {
    return "D"; //"danger";
  } else if (hi >= 125){
    return "E"; //"E danger";
  }
}

// Function to display sensor data on LCD and send to Blynk
void displaySensorData() {
  const uint32_t minI2CFreq = 100000;
  const uint32_t maxI2CFreq = 600000;
  const uint32_t freqStep = 50000;

  float temperature = NAN;
  float humidity = NAN;
  bool readSuccess = false;

  // Try different I2C frequencies until a successful read
  for (uint32_t i2cFreq = minI2CFreq; i2cFreq <= maxI2CFreq; i2cFreq += freqStep) {
    Wire.setClock(i2cFreq);
    unsigned long start = micros();
    sht.read();
    unsigned long stop = micros();

    // Check if the read was successful
    if (!isnan(sht.getTemperature()) && !isnan(sht.getHumidity())) {
      temperature = sht.getTemperature();
      humidity = sht.getHumidity();
      readSuccess = true;
      break;
    }

    Serial.print(i2cFreq / 1000);
  }

  // If no successful read occurred, display an error message
  if (!readSuccess) {
    Serial.println("Failed to read from SHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to read");
    lcd.setCursor(0, 1);
    lcd.print("from SHT20 sensor");
    return;
  }

  // Read MQ-137 sensor
  float ammonia = readMQ137();

  // Calculate Heat Index
  float heatIndex = calculateHeatIndex(temperature, humidity);
  String hiCategory = categorizeHeatIndex(heatIndex);

  // Print data to Serial Monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Ammonia: ");
  Serial.print(ammonia);
  Serial.print(" ppm, Heat Index: ");
  Serial.print(heatIndex);
  Serial.print(" (");
  Serial.print(hiCategory);
  Serial.println(")");

  // Display data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum : ");
  lcd.print(humidity);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("NH3 : ");
  lcd.print(ammonia);
  lcd.print(" ppm");
  lcd.setCursor(0, 3);
  lcd.print("HI  : ");
  lcd.print(heatIndex);
  lcd.print(" F ");
  lcd.print(hiCategory);

  // Send data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, heatIndex);
  Blynk.virtualWrite(V3, ammonia);
  Blynk.virtualWrite(V5, hiCategory);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Wire.begin();
  sht.begin();

  // Initialize SHT20 sensor
  uint8_t stat = sht.getStatus();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  static unsigned long lastDisplayTime = 0;
  unsigned long currentMillis = millis();

  Blynk.run(); // Run Blynk

  if (currentMillis - lastDisplayTime >= 1000) {
    displaySensorData();
    lastDisplayTime = currentMillis;
  }
}
