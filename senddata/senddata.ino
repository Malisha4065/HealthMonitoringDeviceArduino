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

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    String message = "Accelerometer Data:\n" + data;
    Serial.println(message);
    bot.sendMessage(chat_id, message, "");

    String dataTemp = Serial.readStringUntil('\n');
    String messageTemp = "Heart Rate:\n" + dataTemp;
    Serial.println(messageTemp);
    bot.sendMessage(chat_id, messageTemp, "");

    String dataTemp2 = Serial.readStringUntil('\n');
    String messageTemp2 = "Temperature:\n" + dataTemp2;
    Serial.println(messageTemp2);
    bot.sendMessage(chat_id, messageTemp2, "");

  }
}
