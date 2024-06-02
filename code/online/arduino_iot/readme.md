# Sensor Data Display and IoT Monitoring System

## About

This project is a sensor data display and IoT monitoring system utilizing an Arduino, an MQ-137 ammonia sensor, an SHT31 temperature and humidity sensor, and an LCD screen. The system reads temperature, humidity, and ammonia levels, calculates the heat index, categorizes it, and displays the data on an LCD screen. Additionally, it sends the data to the Arduino IoT Cloud for remote monitoring. This system provides real-time environmental data monitoring, useful for applications in agriculture, industrial safety, and indoor air quality monitoring.

## Structure

The project structure includes:

1. **Hardware Components**:
   - Arduino board
   - MQ-137 ammonia sensor
   - SHT31 temperature and humidity sensor
   - LCD screen (20x4) with I2C interface

2. **Libraries**:
   - `Wire.h`: For I2C communication.
   - `Adafruit_Sensor.h`: Base class for sensor libraries.
   - `Adafruit_SHT31.h`: For interfacing with the SHT31 sensor.
   - `LiquidCrystal_I2C.h`: For controlling the I2C LCD.
   - `ArduinoIoTCloud.h`: For IoT Cloud connectivity.
   - `Arduino_ConnectionHandler.h`: For managing the connection to the IoT Cloud.

3. **Code Files**:
   - `main.ino`: Contains the setup and loop functions along with helper functions for reading sensors, calculating heat index, displaying data, and updating the IoT Cloud.

## Explanation

### Initialization

- **SHT31 Sensor**: Initialized with the I2C address `0x44`. The sensor reads temperature and humidity.
- **LCD**: Initialized with the I2C address `0x27` for a 20x4 character display.
- **MQ-137 Sensor**: Connected to analog pin `34`, it measures ammonia concentration in ppm (parts per million).
- **IoT Cloud**: Configured with WiFi credentials and device keys for connecting to the Arduino IoT Cloud.

### Functions

1. **readMQ137()**: Reads the analog value from the MQ-137 sensor and converts it to ppm.
2. **calculateHeatIndex(float temperature, float humidity)**: Uses a simplified formula to calculate the heat index based on temperature and humidity.
3. **categorizeHeatIndex(float hi)**: Categorizes the heat index into "Normal", "Ideal", "Tolerable", or "Extreme".
4. **displaySensorData()**: Reads data from the sensors, calculates the heat index, displays all the information on the LCD, and updates the IoT Cloud.

### Main Program Flow

- **setup()**: Initializes serial communication, the SHT31 sensor, the LCD, and the IoT Cloud connection. Sets up the initial conditions.
- **loop()**: Periodically (every second) reads sensor data, updates the LCD display, and sends data to the IoT Cloud.

## Use Case

This system can be deployed in various scenarios requiring environmental monitoring:
- **Agriculture**: Monitor greenhouse conditions to ensure optimal growth environments.
- **Industrial Safety**: Detect harmful ammonia levels to protect workers.
- **Indoor Air Quality**: Monitor and improve indoor environments for health and comfort.

## Additional Information

### Calibration

- **MQ-137 Calibration**: The conversion from analog value to ppm is simplified. For accurate results, calibrate the sensor according to the manufacturer's guidelines and adjust the conversion formula.

### Heat Index Calculation

- The heat index calculation is a simplified version. For precise applications, consider using more comprehensive formulas considering additional factors like wind speed and solar radiation.

### Error Handling

- The system checks for failed reads from the SHT31 sensor and prints an error message to the serial monitor if the sensor read fails.

### Expansion

- **Additional Sensors**: Easily expandable to include more sensors (e.g., CO2, dust, other gas sensors).
- **Data Logging**: Integrate with SD card modules or other IoT platforms for data logging and remote monitoring.

## Requirements

- **Software**: Arduino IDE with the following libraries installed:
  - Adafruit SHT31 Library
  - LiquidCrystal I2C Library
  - ArduinoIoTCloud Library
  - Arduino_ConnectionHandler Library
- **Hardware**:
  - Arduino board
  - MQ-137 sensor
  - SHT31 sensor
  - LCD display (20x4 with I2C interface)
  - Connecting wires and breadboard
  - Internet connection for IoT Cloud functionality

## Installation

1. Connect the sensors and LCD to the Arduino as per the pin configuration in the code.
2. Install the required libraries via the Arduino Library Manager.
3. Set up your Thing on the Arduino IoT Cloud, get the `DEVICE_KEY` and `DEVICE_LOGIN_NAME`, and replace the placeholders in the code with your credentials.
4. Upload the provided code to the Arduino using the Arduino IDE.
5. Open the serial monitor to debug and verify sensor readings.
6. Monitor your sensor data remotely via the Arduino IoT Cloud dashboard.

This README provides an overview of the project, helping users understand its functionality, setup, and potential applications. For further customization and optimization, refer to the sensor datasheets and library documentation.
