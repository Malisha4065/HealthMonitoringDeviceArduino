#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

char ssid[] = "Dushmin";    
char password[] = "123456789"; 
#define BOTtoken "7265419879:AAFskZZ8JUehZfJCK-l2dJwIO_BSMgC9FHU"  

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const char* chat_id = "1031909365"; 

void setup() {
  Serial.begin(9600); 

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

  client.setInsecure();
}

String accelerometerData = "";
String heartRateData = "";
String temperatureData = "";
String oxygenData = "";

void loop() {
  if (Serial.available() > 0) {
  String data = Serial.readStringUntil('\n');
  if (data.startsWith("A:")) {
    accelerometerData = data.substring(2);
  } else if (data.startsWith("H:")) {
    heartRateData = data.substring(2);
  } else if (data.startsWith("T:")) {
    temperatureData = data.substring(2);
  } else if (data.startsWith("O:")) {
    oxygenData = data.substring(2);
  } else if (data.startsWith("E:")) {
    String emergency = data.substring(2);
    String sendEmergency = "🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑\n";
    sendEmergency += emergency + "\n";
    bot.sendMessage(chat_id, sendEmergency, "");
  }

  }
  if (accelerometerData != "" && heartRateData != "" && temperatureData != "") {
      String message = "Accelerometer Data:\n" + accelerometerData + "\n\n";

      if (oxygenData == "") {
        message += "No Heart Beat Detected\n\n";
      } else {
        message += "Heart Rate:\n" + heartRateData + "\n\n";
        message += "Blood Oxygen:\n" + oxygenData + "\n\n"; 
      }
      message += "Temperature:\n" + temperatureData + "\n";

      bot.sendMessage(chat_id, message, "");

      // Clear the data after sending
      accelerometerData = "";
      heartRateData = "";
      temperatureData = "";
      oxygenData = "";
    }
}
