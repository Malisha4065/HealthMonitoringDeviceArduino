
/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;


uint32_t tsLastReport = 0;

// For accelerometer
const int xPin = A1;
const int yPin = A2;
const int zPin = A3;
// lcd pin initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(9600);
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(zPin, INPUT);

    Serial.print("Initializing pulse oximeter..");
    lcd.begin(16, 2);
    lcd.print("pulse..");
    delay(1000);
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
    float heartRate = 0;
    uint8_t spo2 = 0;
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        heartRate = pox.getHeartRate();
        Serial.print(heartRate);
        Serial.print("bpm / SpO2:");
        spo2 = pox.getSpO2();
        Serial.print(spo2);
        Serial.println("%");

        tsLastReport = millis();
    }

    if (heartRate == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No beat detected");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Heart rate:");
      lcd.setCursor(0, 1);
      lcd.print(heartRate);
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("bpm / SpO2:");
      lcd.setCursor(0, 1);
      lcd.print(spo2);
      lcd.println("%");
      delay(1000);
    }
}
