/*
 * Virtuino MQTT getting started example
 * Broker: HiveMQ (Secure connection)
 * Supported boards: ESP8266 / ESP32 
 * Created by Ilias Lamprou
 * Jul 13 2021
 */

#ifdef ESP8266
 #include <ESP8266WiFi.h>  // Pins for board ESP8266 Wemos-NodeMCU
 #else
 #include <WiFi.h>  
#endif
 
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//---- WiFi settings
const char* ssid = "";
const char* password = "";

//---- MQTT Broker settings
const char* mqtt_server = ""; // replace with your broker url
const char* mqtt_username = "";
const char* mqtt_password = "";
const int mqtt_port =8883;

 

WiFiClientSecure espClient;   // for no secure connection use WiFiClient instead of WiFiClientSecure 
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];


static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";


//==========================================
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}


//=====================================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe("Connect");   // subscribe the topics here
      client.publish("connect", "esp8266Connected");   // subscribe the topics here
      client.subscribe("device/status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//================================================ setup
//================================================
void setup() {
\
  Serial.begin(115200);
  while (!Serial) delay(1);
  setup_wifi();
     // Initialize the BUILTIN_LED pin as an output


  #ifdef ESP8266
    espClient.setInsecure();
  #else   // for the ESP32
    espClient.setCACert(root_ca);      // enable this line and the the "certificate" code for secure connection
  #endif
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


//================================================ loop
//================================================
void loop() {

  if (!client.connected()) reconnect();
  client.loop();
  //
}

//=======================================  
// This void is called every time we have a message from the broker

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  if (String(topic) == "device/status") {
    Serial.print("Changing output to ");
   }

}



//======================================= publising as strings
void publishMessage(const char* topic, String payload , boolean retained){
  client.publish(topic, (byte*) payload.c_str(), 10, true);
  Serial.println("Message publised ["+String(topic)+"]: "+payload);
}
