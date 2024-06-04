#include <LiquidCrystal.h>

const int xPin = A1;
const int yPin = A2;
const int zPin = A3;

// lcd pin initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600); // Initialize serial communication with D1 Mini
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("hello, world!");
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

  // Display the values on the LCD
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first column (0) of the first row (0)
  lcd.print("Accelerometer Data");

  lcd.setCursor(0, 1); // Set cursor to the first column (0) of the second row (1)
  lcd.print("X:");
  lcd.print(x);
  lcd.print(" Y:");
  lcd.print(y);
  lcd.print(" Z:");
  lcd.print(z);

  delay(5000); // Wait for 1 minute
}