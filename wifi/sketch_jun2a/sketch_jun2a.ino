#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

char ssid[] = "Dialog 4G 447";     // your network SSID (name)
char password[] = "D9E83017"; // your network keyD9E83017
#define BOTtoken "7265419879:AAFskZZ8JUehZfJCK-l2dJwIO_BSMgC9FHU"  // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages’ scan has been done
bool Start = false;
int ledStatus = 0;
void handleNewMessages(int numNewMessages) {
 Serial.print("led status = ");
 Serial.print(String(numNewMessages));
 Serial.println(",pesan berhasil diterima\n");
 for (int i=0; i<numNewMessages; i++) {
   String chat_id = String(bot.messages[i].chat_id);
   String text = bot.messages[i].text;
   String from_name = bot.messages[i].from_name;
   if (from_name == "") from_name = "Guest";
   if (text == "/ledoff") {
     ledStatus = 0;
     digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off (LOW is the voltage level)
     bot.sendMessage(chat_id, "Led is OFF", "");
   }
   else if (text == "/ledon") {
     digitalWrite(LED_BUILTIN, LOW);    // turn the LED off (LOW is the voltage level)
     ledStatus = 1;
     bot.sendMessage(chat_id, "Led is ON", "");
   }
   else if (text == "/status") {
     if(ledStatus){
       bot.sendMessage(chat_id, "Led is ON", "");
     } else {
       bot.sendMessage(chat_id, "Led is OFF", "");
     }
   }
   else if (text == "/help") {
     String welcome = "Hi " + from_name + " :) \n";
     welcome += "/ledon   : Turn on led\n";
     welcome += "/ledoff  : Turn off led\n";
     welcome += "/status  : Check led status\n";
     bot.sendMessage(chat_id, welcome, "Markdown");
   }
   else {
     String invalid = "Hi " + from_name + " :) \n";
     invalid += "Your command is invalid \n";
     invalid += "/ledon   : Turn on led\n";
     invalid += "/ledoff  : Turn off led\n";
     invalid += "/status  : Check led status\n";
     bot.sendMessage(chat_id, invalid, "Markdown");
   }
   }
 }
void setup() {
 Serial.begin(9600);
 pinMode(LED_BUILTIN, OUTPUT);

 // Set WiFi to station mode and disconnect from an AP if it was Previously
 // connected
 WiFi.mode(WIFI_STA);
 WiFi.disconnect();
 delay(100);
 // attempt to connect to Wifi network:
 Serial.print("Connecting Wifi: ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
 }
 Serial.println("");
 Serial.println("WiFi Connected");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 delay(10);
 //Bypass certificate validation for testing purposes
 client.setInsecure();
 digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off (LOW is the voltage level)
}
void loop() {
 if (millis() > Bot_lasttime + Bot_mtbs)  {
   int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
   while(numNewMessages) {
     Serial.println("Pesan Diterima");
     Serial.println("sedang diproses.....");
     handleNewMessages(numNewMessages);
     numNewMessages = bot.getUpdates(bot.last_message_received + 1);
   }
   Bot_lasttime = millis();
 }
}