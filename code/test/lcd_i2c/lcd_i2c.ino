#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the library with the I2C address and the dimensions of the LCD
// Uncomment the appropriate line for your LCD size

// For 16x2 LCD
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// For 20x4 LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  lcd.setCursor(0, 0); // Set the cursor to the first column, first row
  lcd.print("I2C LCD Test"); // Print a message to the LCD

  lcd.setCursor(0, 1); // Set the cursor to the first column, second row
  lcd.print("Line 2 here!"); // Print a message to the LCD

  // Uncomment these lines for a 20x4 LCD
  lcd.setCursor(0, 2); // Set the cursor to the first column, third row
  lcd.print("Line 3 here!"); // Print a message to the LCD

  lcd.setCursor(0, 3); // Set the cursor to the first column, fourth row
  lcd.print("Line 4 here!"); // Print a message to the LCD
}

void loop() {
  // No need to repeat anything, the setup does the initial testing
}
