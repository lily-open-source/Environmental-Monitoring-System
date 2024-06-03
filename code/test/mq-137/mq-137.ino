const int MQ137_PIN = 34; // Define the analog input pin for MQ-137 (GPIO 34 is used as an example)

void setup() {
  Serial.begin(9600); // Initialize Serial Monitor at 9600 baud rate
  Serial.println("MQ-137 Sensor Reading");
}

void loop() {
  int analogValue = analogRead(MQ137_PIN); // Read the analog value from the MQ-137 sensor
  float voltage = analogValue * (3.3 / 4095.0); // Convert the analog value to voltage (ESP32 ADC resolution is 12 bits: 0-4095)

  // Print the values to the Serial Monitor
  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  delay(1000); // Wait for 1 second before reading again
}
