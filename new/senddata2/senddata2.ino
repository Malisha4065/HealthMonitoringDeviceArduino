#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

char ssid[] = "Dialog 4G 447";     // your network SSID (name)
char password[] = "D9E83017"; // your network keyD9E83017
#define BOTtoken "7265419879:AAFskZZ8JUehZfJCK-l2dJwIO_BSMgC9FHU"  

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const char* chat_id = "1031909365"; // Replace with your chat ID

void setup() {
  Serial.begin(9600); // Initialize serial communication with Arduino Uno

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Bypass certificate validation for testing purposes
  client.setInsecure();
}

String accelerometerData = "";
String heartRateData = "";
String temperatureData = "";

void loop() {
  if (Serial.available() > 0) {
  String data = Serial.readStringUntil('\n');
  if (data.startsWith("A:")) {
    accelerometerData = data.substring(2);
  } else if (data.startsWith("H:")) {
    heartRateData = data.substring(2);
  } else if (data.startsWith("T:")) {
    temperatureData = data.substring(2);
  }

  }
  if (accelerometerData != "" && heartRateData != "" && temperatureData != "") {
      String message = "Accelerometer Data:\n" + accelerometerData + "\n";
      message += "Heart Rate:\n" + heartRateData + "\n";
      message += "Temperature:\n" + temperatureData + "\n";

      bot.sendMessage(chat_id, message, "");

      // Clear the data after sending
      accelerometerData = "";
      heartRateData = "";
      temperatureData = "";
    }
}
