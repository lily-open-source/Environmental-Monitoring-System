import network
import blynklib
import time
from machine import I2C, Pin, ADC, UART
from esp8266_i2c_lcd import I2cLcd
import adafruit_sht31d

# Define your Blynk auth token
auth = 'YourAuthToken'

# Your WiFi credentials
ssid = 'YourNetworkName'
password = 'YourPassword'

# Initialize WiFi connection
wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect(ssid, password)
while not wifi.isconnected():
    time.sleep(1)

# Initialize Blynk
blynk = blynklib.Blynk(auth)

# Initialize I2C for SHT31 sensor and LCD
i2c = I2C(scl=Pin(22), sda=Pin(21), freq=100000)
sht31 = adafruit_sht31d.SHT31D(i2c)
lcd = I2cLcd(i2c, 0x27, 2, 16)

# Define the pin for MQ-137
mq137_pin = ADC(Pin(34))
mq137_pin.atten(ADC.ATTN_11DB)  # set the input attenuation to 11dB

# Initialize MAX485 control pins
MAX485_DE = Pin(4, Pin.OUT)
MAX485_RE = Pin(4, Pin.OUT)

# Initialize RS-485 Serial
uart = UART(2, baudrate=9600, tx=17, rx=16)

# Function to read MQ-137 sensor value
def read_mq137():
    analog_value = mq137_pin.read()
    ppm = (analog_value / 4095.0) * 100  # Simplified conversion
    return ppm

# Function to calculate heat index
def calculate_heat_index(temperature, humidity):
    return ((1.8 * temperature) + humidity + 32) * 1.8

# Function to categorize heat index
def categorize_heat_index(hi):
    if 145 <= hi <= 149:
        return "Normal"
    elif 150 <= hi <= 155:
        return "Ideal"
    elif 156 <= hi <= 160:
        return "Tolerable"
    else:
        return "Extreme"

# Function to send sensor data to Blynk, display on LCD, and transmit over RS-485
def send_sensor_data():
    temperature = sht31.temperature
    humidity = sht31.relative_humidity
    ammonia = read_mq137()
    heat_index = calculate_heat_index(temperature, humidity)
    hi_category = categorize_heat_index(heat_index)

    blynk.virtual_write(0, temperature)
    blynk.virtual_write(1, humidity)
    blynk.virtual_write(2, ammonia)
    blynk.virtual_write(3, heat_index)
    blynk.virtual_write(4, hi_category)

    print(f"Temperature: {temperature} °C, Humidity: {humidity} %, Ammonia: {ammonia} ppm, Heat Index: {heat_index} ({hi_category})")

    lcd.clear()
    lcd.move_to(0, 0)
    lcd.putstr(f"T:{temperature}C H:{humidity}%")
    lcd.move_to(0, 1)
    lcd.putstr(f"NH3:{ammonia} HI:{heat_index}")

    MAX485_RE.value(1)
    MAX485_DE.value(1)
    uart.write(f"Temperature: {temperature} °C, Humidity: {humidity} %, Ammonia: {ammonia} ppm, Heat Index: {heat_index} ({hi_category})\n")
    MAX485_RE.value(0)
    MAX485_DE.value(0)

# Setup timer to send data every second
def main():
    while True:
        blynk.run()
        send_sensor_data()
        time.sleep(1)

if __name__ == "__main__":
    main()
