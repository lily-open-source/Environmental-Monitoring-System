#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

// Define the pin for MQ-137
const int mq137Pin = 34;

// Initialize SHT31 sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Initialize the LCD, set the I2C address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// WiFi credentials
const char SSID[] = "your_SSID";
const char PASS[] = "your_PASSWORD";

// Arduino IoT Cloud credentials
const char DEVICE_LOGIN_NAME[] = "your_device_login_name";
const char DEVICE_KEY[] = "your_device_key";

// Variables to be sent to the IoT cloud
float temperature;
float humidity;
float ammonia;
float heatIndex;
String hiCategory;

// Function to read MQ-137 sensor value
float readMQ137() {
  int analogValue = analogRead(mq137Pin);
  // Convert analog value to ppm (calibration needed)
  float ppm = (analogValue / 4095.0) * 100; // Simplified conversion
  return ppm;
}

// Function to calculate heat index
float calculateHeatIndex(float temperature, float humidity) {
  // Using the simplified formula for heat index calculation
  float hi = ((1.8 * temperature) + humidity + 32) * 1.8;
  return hi;
}

// Function to categorize heat index
String categorizeHeatIndex(float hi) {
  if (hi >= 145 && hi <= 149) {
    return "Normal";
  } else if (hi >= 150 && hi <= 155) {
    return "Ideal";
  } else if (hi >= 156 && hi <= 160) {
    return "Tolerable";
  } else {
    return "Extreme";
  }
}

// Function to display sensor data on LCD
void displaySensorData() {
  // Read SHT31 sensor
  temperature = sht31.readTemperature();
  humidity = sht31.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from SHT31 sensor!");
    return;
  }

  // Read MQ-137 sensor
  ammonia = readMQ137();

  // Calculate Heat Index
  heatIndex = calculateHeatIndex(temperature, humidity);
  hiCategory = categorizeHeatIndex(heatIndex);

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
  lcd.print(" ");
  lcd.print(hiCategory);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize SHT31 sensor
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Setup IoT Cloud connection
  ArduinoCloud.begin(SSID, PASS);

  // Connect to the IoT Cloud using the provided credentials
  ArduinoCloud.addDevice(DEVICE_LOGIN_NAME, DEVICE_KEY);

  // Define the properties in the cloud
  ArduinoCloud.addProperty(temperature, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(ammonia, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(heatIndex, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(hiCategory, READ, 1 * SECONDS, NULL);

  // Setup a timer to display data every second
  // Note: Arduino does not have built-in timers like BlynkTimer, so we use millis() for timing
}

void loop() {
  static unsigned long lastDisplayTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastDisplayTime >= 1000) { // 1 second interval
    displaySensorData();
    lastDisplayTime = currentMillis;
  }

  // Update the cloud
  ArduinoCloud.update();
}
