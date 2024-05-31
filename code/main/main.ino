#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "YourTemplateName"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
#include <LiquidCrystal_I2C.h>

// Define your Blynk auth token
char auth[] = "YourAuthToken";

// Your WiFi credentials
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

// Initialize SHT31 sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Define the pin for MQ-137
const int mq137Pin = 34;

// Initialize the LCD, set the I2C address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define MAX485 control pins
const int MAX485_DE = 4;
const int MAX485_RE = 4;

// Initialize RS-485 Serial
HardwareSerial RS485Serial(2);

// Timer to send data to Blynk
BlynkTimer timer;

// Function to read MQ-137 sensor value
float readMQ137() {
  int analogValue = analogRead(mq137Pin);
  // Convert analog value to ppm (calibration needed)
  float ppm = (analogValue / 4095.0) * 100; // Simplified conversion
  return ppm;
}

// Function to calculate heat index
float calculateHeatIndex(float temperature, float humidity) {
  return ((1.8 * temperature) + humidity + 32) * 1.8;
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

// Function to send sensor data to Blynk, display on LCD, and transmit over RS-485
void sendSensorData() {
  // Read SHT31 sensor
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  // Read MQ-137 sensor
  float ammonia = readMQ137();

  // Calculate Heat Index
  float heatIndex = calculateHeatIndex(temperature, humidity);
  String hiCategory = categorizeHeatIndex(heatIndex);

  // Send data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, ammonia);
  Blynk.virtualWrite(V3, heatIndex);
  Blynk.virtualWrite(V4, hiCategory);

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
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("NH3:");
  lcd.print(ammonia);
  lcd.print(" HI:");
  lcd.print(heatIndex);

  // Transmit data over RS-485
  digitalWrite(MAX485_RE, HIGH);
  digitalWrite(MAX485_DE, HIGH);
  RS485Serial.print("Temperature: ");
  RS485Serial.print(temperature);
  RS485Serial.print(" °C, Humidity: ");
  RS485Serial.print(humidity);
  RS485Serial.print(" %, Ammonia: ");
  RS485Serial.print(ammonia);
  RS485Serial.print(" ppm, Heat Index: ");
  RS485Serial.print(heatIndex);
  RS485Serial.print(" (");
  RS485Serial.print(hiCategory);
  RS485Serial.println(")");
  digitalWrite(MAX485_RE, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Initialize SHT31 sensor
  if (!sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31 sensor!");
    while (1) delay(1);
  }

  // Initialize LCD
  lcd.begin(16, 2); // Corrected method call with parameters
  lcd.backlight();

  // Initialize MAX485
  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE, LOW);
  digitalWrite(MAX485_DE, LOW);
  RS485Serial.begin(9600, SERIAL_8N1, 16, 17); // Initialize RS-485 Serial with TX (GPIO17) and RX (GPIO16)

  // Setup timer to send data every second
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Run timer
  timer.run();
}
