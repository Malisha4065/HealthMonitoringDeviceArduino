#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS 1000

// For Pulse oximeter
PulseOximeter pox;
uint32_t tsLastReport = 0;

// For accelerometer
const int xPin = A1;
const int yPin = A2;
const int zPin = A3;

// LCD pin initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables to store the current time and the time when the LCD was last updated
unsigned long currentMillis, previousBeatMillis = 0, previousAccelMillis = 0, previousHeartRateMillis = 0;

// The desired intervals (in milliseconds) for LCD updates
const unsigned long LCD_BEAT_INTERVAL = 5000; // 3 seconds
const unsigned long LCD_ACCEL_INTERVAL = 5000; // 5 seconds
const unsigned long LCD_HEART_RATE_INTERVAL = 5000; // 1 second

void onBeatDetected()
{
    previousBeatMillis = millis(); // Update the last beat time
}

void setup() {
  Serial.begin(9600); // Initialize serial communication with D1 Mini
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);
  lcd.begin(16, 2);
  lcd.print("pulse..");

  if (!pox.begin()) {
    lcd.clear();
    lcd.print("PULSE FAILED");
    while (1); // Halt the program
  } else {
    lcd.clear();
    lcd.print("PULSE SUCCESS");
    previousBeatMillis = millis(); // Initialize the last beat time
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  pox.update();
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);

  // Send the data to D1 Mini
  // Serial.print(x);
  // Serial.print(",");
  // Serial.print(y);
  // Serial.print(",");
  // Serial.println(z);

  currentMillis = millis(); // Get the current time

  // Check if it's time to update the accelerometer display
  if (currentMillis - previousAccelMillis >= LCD_ACCEL_INTERVAL) {
    previousAccelMillis = currentMillis; // Update the last accelerometer update time

    // Display the accelerometer values on the LCD
    lcd.clear(); // Clear the LCD display
    lcd.setCursor(0, 0); // Set cursor to the first column (0) of the first row (0)
    lcd.print("Accelerometer: ");
    lcd.setCursor(0, 1); // Set cursor to the first column (0) of the second row (1)
    lcd.print(x);
    lcd.print(" ");
    lcd.print(y);
    lcd.print(" ");
    lcd.print(z);
  }

  // Check if it's time to update the heart rate display
  if (currentMillis - previousHeartRateMillis >= LCD_HEART_RATE_INTERVAL) {
    previousHeartRateMillis = currentMillis; // Update the last heart rate update time

    if (currentMillis - tsLastReport > REPORTING_PERIOD_MS) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Heart rate:");
      lcd.setCursor(0, 1);
      lcd.print(pox.getHeartRate());

      tsLastReport = currentMillis;
    }
  }

  // Check if it's time to display the beat message
  // if (currentMillis - previousBeatMillis < LCD_BEAT_INTERVAL) {
  //   lcd.clear();
  //   lcd.print("Beat!");
  //   Serial.println("Beat");
  // }
}