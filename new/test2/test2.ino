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

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int xMin = 272;
int xMax = 408;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int yMin = 269;
int yMax = 408;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int zMin = 287;
int zMax = 430;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM

// thresholds
const float SHOCK_THRESHOLD = 2.0;
const int TEMP_THRESHOLD = 38;


// For DHT11 sensor
dht tempSensor;
const int dhtPin = 3;

// LCD pin initialization
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long currentMillis, previousBeatMillis = 0, previousDisplayMillis = 0;

const unsigned long LCD_DISPLAY_INTERVAL = 5000;

int displayState = 0; // 0 for accelerometer, 1 for heart rate, 2 for temperature

void onBeatDetected()
{
  previousBeatMillis = millis(); 
}

void setup() {
  Serial.begin(9600); 
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);
  lcd.begin(16, 2);
  lcd.print("pulse..");

  if (!pox.begin()) {
    lcd.clear();
    lcd.print("PULSE FAILED");
    while (1); 
  } else {
    lcd.clear();
    lcd.print("PULSE SUCCESS");
    previousBeatMillis = millis(); 
    previousDisplayMillis = millis(); 
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

bool shock = false;

void loop() {
  pox.update();
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);

  long xMilliG = map(x, xMin, xMax, -1000, 1000);
  long yMilliG = map(y, yMin, yMax, -1000, 1000);
  long zMilliG = map(z, zMin, zMax, -1000, 1000);

  float x_g_value = xMilliG / 1000.0;
  float y_g_value = yMilliG / 1000.0;
  float z_g_value = zMilliG / 1000.0;
  
  if (abs(x_g_value) >= SHOCK_THRESHOLD || abs(y_g_value) >= SHOCK_THRESHOLD || abs(z_g_value) >= SHOCK_THRESHOLD) {
    shock = true;
  }

  currentMillis = millis(); 

  if (currentMillis - previousDisplayMillis >= LCD_DISPLAY_INTERVAL) {
    previousDisplayMillis = currentMillis; 

    lcd.clear();

    if (displayState == 0) {
      lcd.setCursor(0, 0);
      if (shock) {
        lcd.print("FALL DETECTED!");
        Serial.println("E:FALL DETECTED!");
        shock = false;
      } else {
        lcd.print("Accelerometer: ");
      }
      
      lcd.setCursor(0, 1);
      lcd.print(x_g_value, 0);
      lcd.print("G ");
      lcd.print(y_g_value, 0);
      lcd.print("G ");
      lcd.print(z_g_value, 0);
      lcd.print("G");

      Serial.print("A:");
      Serial.print(x_g_value, 0);
      Serial.print("G");
      Serial.print(",");
      Serial.print(y_g_value, 0);
      Serial.print("G");
      Serial.print(",");
      Serial.print(z_g_value, 0);
      Serial.println("G");

      displayState = 1; 
    } else if (displayState == 1) {
      lcd.setCursor(0, 0);
      float heartRate = pox.getHeartRate();
      float spo2 = pox.getSpO2();
      bool nobeat = false;
      heartRate -= 90;  // calibration
      if (heartRate > 100) {
        lcd.print("High Heart Rate!");
        Serial.println("E:HIGH HEART RATE DETECTED!");
      } else if (heartRate > 60) {
        lcd.print("Heart rate:");
      } else if (heartRate > 0) {
        lcd.print("Low Heart Rate!");
        Serial.println("E:LOW HEART RATE DETECTED!");
      } else {
        lcd.print("No HeartBeat");
        heartRate = 0;
        nobeat = true;
      }

      lcd.setCursor(0, 1);
      lcd.print(heartRate, 0);
      lcd.print(" bpm ");
      if (!nobeat) {
        lcd.print(" ");
        lcd.print(spo2);
        lcd.print("%");
        Serial.print("O:");
        Serial.print(spo2);
        Serial.println("%");
      }

      Serial.print("H:");
      Serial.print(heartRate, 0);
      Serial.println(" bpm");

      displayState = 2; 
    } else if (displayState == 2) {
      int val = tempSensor.read11(dhtPin);
      if (val == -1) {
        double temperature = tempSensor.temperature;
        lcd.setCursor(0, 0);
        if (temperature > TEMP_THRESHOLD) {
          lcd.print("FEVER DETECTED!");
          Serial.println("E:FEVER DETECTED!");
        } else {
          lcd.print("Temperature:");
        }
        lcd.setCursor(0, 1);
        lcd.print(temperature);
        lcd.print("C");

        Serial.print("T:");
        Serial.print(temperature);
        Serial.println(" C");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("No temp detected");
      }

      displayState = 0; 
    }
  }
}
