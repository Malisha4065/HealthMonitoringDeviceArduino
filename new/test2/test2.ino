#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "dht.h"

#define REPORTING_PERIOD_MS 1000

// For Pulse oximeter
PulseOximeter pox;
uint32_t tsLastReport = 0;

// For accelerometer
const int xPin = A1;
const int yPin = A2;
const int zPin = A3;

// For DHT11 sensor
dht tempSensor;
const int dhtPin = 7;

// LCD pin initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables to store the current time and the time when the LCD was last updated
unsigned long currentMillis, previousBeatMillis = 0, previousDisplayMillis = 0;

// The desired intervals (in milliseconds) for LCD updates
const unsigned long LCD_DISPLAY_INTERVAL = 5000; // 5 seconds

int displayState = 0; // 0 for accelerometer, 1 for heart rate, 2 for temperature

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
    previousDisplayMillis = millis(); // Initialize the last display update time
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

  // Check if it's time to update the LCD display
  if (currentMillis - previousDisplayMillis >= LCD_DISPLAY_INTERVAL) {
    previousDisplayMillis = currentMillis; // Update the last display update time

    lcd.clear(); // Clear the LCD display

    if (displayState == 0) {
      // Display the accelerometer values on the LCD
      lcd.setCursor(0, 0); // Set cursor to the first column (0) of the first row (0)
      lcd.print("Accelerometer: ");
      lcd.setCursor(0, 1); // Set cursor to the first column (0) of the second row (1)
      lcd.print(x);
      lcd.print(" ");
      lcd.print(y);
      lcd.print(" ");
      lcd.print(z);

      Serial.print("A:");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.print(",");
      Serial.println(z);

      displayState = 1; // Next state will be to display heart rate
    } else if (displayState == 1) {
      // Display the heart rate on the LCD
      lcd.setCursor(0, 0);
      lcd.print("Heart rate:");
      lcd.setCursor(0, 1);
      float heartRate = pox.getHeartRate();
      lcd.print(heartRate);

      Serial.print("H:");
      Serial.println(heartRate);

      displayState = 2; // Next state will be to display temperature
    } else if (displayState == 2) {
      // Display the temperature on the LCD
      int val = tempSensor.read11(dhtPin);
      if (val == -1) {
        lcd.setCursor(0, 0);
        lcd.print("Temperature:");
        lcd.setCursor(0, 1);
        double temperature = tempSensor.temperature;
        lcd.print(temperature);
        lcd.print("C");

        Serial.print("T:");
        Serial.println(temperature);
      }

      displayState = 0; // Next state will be to display accelerometer values
    }
  }

  // Check if it's time to display the beat message
  // if (currentMillis - previousBeatMillis < LCD_BEAT_INTERVAL) {
  //   lcd.clear();
  //   lcd.print("Beat!");
  //   Serial.println("Beat");
  // }
}
