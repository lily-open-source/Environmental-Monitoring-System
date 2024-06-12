#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SHT2x.h"
#include "AdafruitIO_WiFi.h"

// Define the pin for MQ-137
const int mq137Pin = 34;

// Initialize SHT20 sensor
uint32_t start, stop;
SHT2x sht;

// Initialize the LCD, set the I2C address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Adafruit IO configuration
#define IO_USERNAME "your_aio_username"
#define IO_KEY "your_aio_key"
#define WIFI_SSID "your_ssid"
#define WIFI_PASS "ssid_password"

// Set up the Adafruit IO client
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Set up Adafruit IO feeds
AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *humidityFeed = io.feed("humidity");
AdafruitIO_Feed *ammoniaFeed = io.feed("ammonia");
AdafruitIO_Feed *heatIndexFeed = io.feed("heat-index");

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
  float hif = -42.379 +
             2.04901523 * tempF +
            10.14333127 * humidity +
            -0.22475541 * tempF * humidity +
            -0.00683783 * pow(tempF, 2) +
            -0.05481717 * pow(humidity, 2) +
             0.00122874 * pow(tempF, 2) * humidity +
             0.00085282 * tempF * pow(humidity, 2) +
            -0.00000199 * pow(tempF, 2) * pow(humidity, 2);
  float hi = (hif - 32) / 1.8; // convert back to Celsius.
  return hi;
}

// Function to categorize heat index
String categorizeHeatIndex(float hi) {
  // <= 27c || 80f
  if (hi <= 27) {
    return "A"; //"safe";
  // >= 27c - 32c || 80f - 90f
  } else if (hi >= 27 && hi <= 32) {
    return "B"; //"caution";
  // >= 32c - 39c || 90f - 103f
  } else if (hi >= 32 && hi <= 39) {
    return "C"; //"E caution";
  // >= 39c - 51c || 103f - 124f
  } else if (hi >= 39 && hi <= 51) {
    return "D"; //"danger";
  // >= 51c || 124f
  } else if (hi >= 51){
    return "E"; //"E danger";
  }
}

// Function to display sensor data on LCD
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
 Serial.print(" °C, (");
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
 lcd.print(" C ");
 lcd.print(hiCategory);
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

 // Connect to Adafruit IO
 io.connect();

 // Wait for a connection
 while (io.status() < AIO_CONNECTED) {
   Serial.print(".");
   delay(500);
 }

 // We are connected
 Serial.println();
 Serial.println(io.statusText());
}

void loop() {
 static unsigned long lastDisplayTime = 0;
 static unsigned long lastSendTime = 0;
 unsigned long currentMillis = millis();

 if (currentMillis - lastDisplayTime >= 1000) {
   displaySensorData();
   lastDisplayTime = currentMillis;
 }

 // Send data to Adafruit IO every 30 seconds
 if (currentMillis - lastSendTime >= 30000) {
   temperatureFeed->save(sht.getTemperature());
   humidityFeed->save(sht.getHumidity());
   ammoniaFeed->save(readMQ137());
   heatIndexFeed->save(calculateHeatIndex(sht.getTemperature(), sht.getHumidity()));
   lastSendTime = currentMillis;
  }

  // Keep Adafruit IO connected
  io.run();
}
