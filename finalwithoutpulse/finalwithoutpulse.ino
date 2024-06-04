#include <LiquidCrystal.h>

// #include <Wire.h>
// #include "MAX30100_PulseOximeter.h"

// #define REPORTING_PERIOD_MS 1000

// // For Pulse oximeter
// PulseOximeter pox;
// uint32_t tsLastReport = 0;

// For accelerometer
const int xPin = A1;
const int yPin = A2;
const int zPin = A3;

// for lm35
float Vout;
float Temp;

// lcd pin initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// void onBeatDetected()
// {
//     lcd.clear();
//     lcd.print("Beat!");
//     delay(3000);
// }

void setup() {
  Serial.begin(9600); // Initialize serial communication with D1 Mini
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("Health Monitor <3");
  delay(1000);
  // lcd.print("pulse..");
  // delay(1000);
  // if (!pox.begin()) {
  //       lcd.clear();
  //       lcd.print("PULSE FAILED");
  //       for(;;);
  // } else {
  //       lcd.clear();
  //       lcd.print("PULSE SUCCESS");
  //       delay(5000);
  // }

  // pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {

  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);

  // Send the data to D1 Mini
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);

  // Display accelerometer values on the LCD
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first column (0) of the first row (0)
  lcd.print("Accelerometer: ");

  lcd.setCursor(0, 1); // Set cursor to the first column (0) of the second row (1)
  lcd.print(x);
  lcd.print(" ");
  lcd.print(y);
  lcd.print(" ");
  lcd.print(z);

  delay(3000);

  // Display temperature values on the LCD
  Vout = analogRead(A0);
  Temp = (Vout * 500)/1023;

  Serial.println(Temp);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.setCursor(0, 1);
  lcd.print(Temp);

  delay(3000); // Wait for 1 minute

  // pox.update();
  // if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
  //       lcd.clear();
  //       lcd.setCursor(0, 0);
  //       lcd.print("Heart rate:");
  //       lcd.setCursor(0, 1);
  //       lcd.print(pox.getHeartRate());
  //       delay(1000);
  //       lcd.clear();
  //       lcd.setCursor(0, 0);
  //       lcd.print("bpm / SpO2:");
  //       lcd.setCursor(0, 1);
  //       lcd.print(pox.getSpO2());
  //       lcd.println("%");
  //       delay(1000);
  //       tsLastReport = millis();
  //   }
  // delay(1000);
}
