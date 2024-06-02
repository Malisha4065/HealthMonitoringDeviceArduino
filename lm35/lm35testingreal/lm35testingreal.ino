float Vout;
float Temp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Vout = analogRead(A0);
  Temp = (Vout * 500)/1023;
  Serial.print("Temperature in Celsius: ");
  Serial.print(Temp);
  Serial.println();
  delay(1000);
}
