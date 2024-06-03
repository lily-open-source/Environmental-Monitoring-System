#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
#include <LiquidCrystal_I2C.h>

// Define the pin for MQ-137
const int mq137Pin = 34;

// Initialize SHT20 sensor
Adafruit_SHT31 sht20 = Adafruit_SHT31();

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
  // Read SHT20 sensor
  float temperature = sht20.readTemperature();
  float humidity = sht20.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from SHT20 sensor!");
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
  lcd.print(" ");
  lcd.print(hiCategory);
}

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize SHT20 sensor
  if (!sht20.begin(0x40)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT20");
    while (1) delay(1);
  }

  // Initialize LCD
  lcd.init();
  lcd.backlight();

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
}
