# Environmental Monitoring System with Adafruit Integration

This project involves creating an environmental monitoring system using an SHT20 temperature and humidity sensor, an MQ-137 ammonia sensor, an LCD display, and Adafruit IO for IoT integration.

## Structure

### Components

- **SHT20 Sensor**: Measures temperature and humidity.
- **MQ-137 Sensor**: Measures ammonia levels in the air.
- **LiquidCrystal_I2C**: LCD display for showing sensor data.
- **AdafruitIO_WiFi**: Connects the system to Adafruit IO for data logging and monitoring.

### Libraries

- `Wire.h`: For I2C communication.
- `LiquidCrystal_I2C.h`: For interfacing with the LCD.
- `SHT2x.h`: For reading data from the SHT20 sensor.
- `AdafruitIO_WiFi.h`: For connecting to Adafruit IO using WiFi.

### Flowchart of the Code

```mermaid
graph TD;
    A[Start] --> B[Initialize components]
    B --> C[Connect to Adafruit IO]
    C --> D[Check IO connection]
    D -- Connected --> E[Read SHT20 sensor data]
    D -- Not Connected --> F[Retry connection]
    E --> G[Check read success]
    G -- Success --> H[Read MQ-137 sensor data]
    G -- Failure --> I[Display error message on LCD]
    H --> J[Calculate heat index]
    J --> K[Categorize heat index]
    K --> L[Display data on LCD]
    L --> M[Send data to Adafruit IO]
    M --> N[Delay 1 second]
    N --> E
```

## Explanation

### Initialization

1. **Serial Communication**: Initialize at 115200 baud rate.
2. **I2C Communication**: Initialize using `Wire.begin()`.
3. **SHT20 Sensor**: Initialize and check status.
4. **LCD**: Initialize and turn on the backlight.
5. **Adafruit IO**: Connect to Adafruit IO using provided credentials and WiFi details.

### Functions

- **readMQ137()**: Reads the analog value from the MQ-137 sensor and converts it to ppm.
- **calculateHeatIndex(float temperature, float humidity)**: Converts temperature to Fahrenheit and calculates the heat index.
- **categorizeHeatIndex(float hi)**: Categorizes the heat index into safety levels.
- **displaySensorData()**: Reads sensor data, calculates heat index, and displays all values on the LCD. Also prints data to the serial monitor.
- **setup()**: Initializes all components and connects to Adafruit IO.
- **loop()**: Continuously reads and displays sensor data and sends data to Adafruit IO at regular intervals.

### Main Program Flow

1. **Setup**: Initializes serial communication, I2C, sensors, LCD, and Adafruit IO.
2. **Loop**: Reads and displays sensor data every second, and sends data to Adafruit IO every 30 seconds.

## Use Case

### Example

- Monitor the temperature, humidity, and ammonia levels in a laboratory, with remote data access via Adafruit IO.
- Use the system in a greenhouse to ensure optimal growing conditions and remotely monitor the data.
- Install the system in a kitchen to monitor air quality and potential ammonia leaks, with IoT integration for alerts.

## Additional Information

### Calibration

Calibrate the MQ-137 sensor for accurate ppm readings. The conversion formula in the code is a simplified version and may need adjustments based on your specific setup and calibration data.

### Error Handling

If the SHT20 sensor fails to read, the system will display an error message on the LCD and print it to the serial monitor.

### Expansion

You can add more sensors or modules by expanding the I2C bus and updating the code accordingly. Additionally, more data feeds can be added to Adafruit IO.

### Requirements

#### Hardware

- Arduino board
- SHT20 sensor
- MQ-137 sensor
- LCD display with I2C interface
- WiFi module (if not built-in)
- Connecting wires

#### Software

- Arduino IDE
- Required libraries: `Wire.h`, `LiquidCrystal_I2C.h`, `SHT2x.h`, `AdafruitIO_WiFi.h`
- Adafruit IO account and credentials

### Installation

1. **Connect the hardware components**: Follow the sensor and LCD pin configurations.
2. **Install the Arduino libraries**: Add the required libraries to your Arduino IDE.
3. **Configure Adafruit IO**: Replace placeholder credentials with your Adafruit IO username, key, and WiFi details.
4. **Upload the code**: Load the provided code onto your Arduino board.
5. **Monitor the output**: View the readings on the LCD and the serial monitor for debugging, and check data logs on Adafruit IO.

**figure:**

![](https://raw.githubusercontent.com/lily-open-source/Environmental-Monitoring-System/main/code/online/adafruit/pic.png)
