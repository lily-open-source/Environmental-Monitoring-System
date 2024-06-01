# IoT Environmental Monitoring System

## About

This project is an Internet of Things (IoT) environmental monitoring system using an Arduino, MQ-137 ammonia sensor, SHT31 temperature and humidity sensor, LCD display, and Adafruit IO for data logging and remote monitoring. It reads temperature, humidity, and ammonia levels, calculates the heat index, categorizes it, and displays the data on an LCD screen while also sending the data to Adafruit IO for remote monitoring and analysis.

## Structure

The project structure includes:

1. **Hardware Components**:
   - Arduino board
   - MQ-137 ammonia sensor
   - SHT31 temperature and humidity sensor
   - LCD screen (20x4) with I2C interface
   - WiFi module for internet connectivity

2. **Libraries**:
   - `Wire.h`: For I2C communication.
   - `Adafruit_Sensor.h`: Base class for sensor libraries.
   - `Adafruit_SHT31.h`: For interfacing with the SHT31 sensor.
   - `LiquidCrystal_I2C.h`: For controlling the I2C LCD.
   - `WiFi.h`: For WiFi connectivity.
   - `AdafruitIO_WiFi.h`: For connecting to Adafruit IO.

3. **Code Files**:
   - `main.ino`: Contains the setup and loop functions along with helper functions for reading sensors, calculating heat index, displaying data, and sending data to Adafruit IO.

## Explanation

### Initialization

- **SHT31 Sensor**: Initialized with the I2C address `0x44`. The sensor reads temperature and humidity.
- **LCD**: Initialized with the I2C address `0x27` for a 20x4 character display.
- **MQ-137 Sensor**: Connected to analog pin `34`, it measures ammonia concentration in ppm (parts per million).
- **WiFi and Adafruit IO**: Configured with user credentials for WiFi and Adafruit IO. Four feeds are created to store temperature, humidity, ammonia, and heat index data.

### Functions

1. **readMQ137()**: Reads the analog value from the MQ-137 sensor and converts it to ppm.
2. **calculateHeatIndex(float temperature, float humidity)**: Uses a simplified formula to calculate the heat index based on temperature and humidity.
3. **categorizeHeatIndex(float hi)**: Categorizes the heat index into "Normal", "Ideal", "Tolerable", or "Extreme".
4. **displayAndSendSensorData()**: Reads data from the sensors, calculates the heat index, displays all the information on the LCD, and sends the data to Adafruit IO.

### Main Program Flow

- **setup()**: Initializes serial communication, the SHT31 sensor, and the LCD. Connects to WiFi and Adafruit IO, and sets up the initial conditions.
- **loop()**: Periodically (every second) reads sensor data, updates the LCD display, sends data to Adafruit IO, and keeps the connection alive with Adafruit IO.

## Use Case

This system can be deployed in various scenarios requiring environmental monitoring and remote data logging:
- **Agriculture**: Monitor greenhouse conditions to ensure optimal growth environments and remotely log data for analysis.
- **Industrial Safety**: Detect harmful ammonia levels to protect workers and provide remote alerts and data logging.
- **Indoor Air Quality**: Monitor and improve indoor environments for health and comfort with remote monitoring capabilities.

## Additional Information

### Calibration

- **MQ-137 Calibration**: The conversion from analog value to ppm is simplified. For accurate results, calibrate the sensor according to the manufacturer's guidelines and adjust the conversion formula.

### Heat Index Calculation

- The heat index calculation is a simplified version. For precise applications, consider using more comprehensive formulas considering additional factors like wind speed and solar radiation.

### Error Handling

- The system checks for failed reads from the SHT31 sensor and prints an error message to the serial monitor if the sensor read fails.

### Expansion

- **Additional Sensors**: Easily expandable to include more sensors (e.g., CO2, dust, other gas sensors).
- **Data Logging and Alerts**: Integrate with more features on Adafruit IO for detailed data logging, analysis, and alert notifications.

## Requirements

- **Software**: Arduino IDE with the following libraries installed:
  - Adafruit SHT31 Library
  - LiquidCrystal I2C Library
  - Adafruit IO Arduino Library
  - WiFi Library
- **Hardware**:
  - Arduino board
  - MQ-137 sensor
  - SHT31 sensor
  - LCD display (20x4 with I2C interface)
  - WiFi module
  - Connecting wires and breadboard

## Installation

1. Connect the sensors and LCD to the Arduino as per the pin configuration in the code.
2. Install the required libraries via the Arduino Library Manager.
3. Configure WiFi and Adafruit IO credentials in the code.
4. Upload the provided code to the Arduino using the Arduino IDE.
5. Open the serial monitor to debug and verify sensor readings and WiFi connection status.

This README provides an overview of the project, helping users understand its functionality, setup, and potential applications. For further customization and optimization, refer to the sensor datasheets, library documentation, and Adafruit IO resources.
