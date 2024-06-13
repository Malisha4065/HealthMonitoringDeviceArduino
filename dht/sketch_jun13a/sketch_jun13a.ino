#include "dht.h"

dht tempSensor;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  int val = tempSensor.read11(7);
  if (val == -1) {
    Serial.print("Temperature: ");
    Serial.println(tempSensor.temperature);
    Serial.print("Humidity: ");
    Serial.println(tempSensor.humidity);
  }
  delay(1000);
}
