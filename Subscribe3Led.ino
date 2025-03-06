#include <WiFi.h>
#include <PubSubClient.h>

#define LED_RED 4
#define LED_GREEN 5
#define LED_WHITE 18

// WiFi
const char *ssid = "4G-MIFI-549"; // Enter your WiFi name
const char *password = "1234567890";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32/led";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

bool ledState1 = false;
bool ledState2 = false;
bool ledState3 = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setupMQTT() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    // Setting LED pin as output
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_WHITE, OUTPUT);

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_WHITE, LOW);

    // Connecting to an MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "ESP32Client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    setupMQTT();
    client.publish(topic, "hello emqx");
    client.subscribe(topic);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!client.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (client.connect(clientId.c_str())) {
        Serial.println("Connected.");
      }      
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char) payload[i];
    }
    Serial.print(message);
    
    if (message == "red-on" && !ledState1) {
        digitalWrite(LED_RED, HIGH);
        ledState1 = true;
    }
    if (message == "red-off" && ledState1) {
        digitalWrite(LED_RED, LOW);
        ledState1 = false;
    }
    if (message == "green-on" && !ledState2) {
        digitalWrite(LED_GREEN, HIGH);
        ledState2 = true;
    }
    if (message == "green-off" && ledState2) {
        digitalWrite(LED_GREEN, LOW);
        ledState2 = false;
    }
    if (message == "white-on" && !ledState3) {
        digitalWrite(LED_WHITE, HIGH);
        ledState3 = true;
    }
    if (message == "white-off" && ledState3) {
        digitalWrite(LED_WHITE, LOW);
        ledState3 = false;
    }
    
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();
    
    if (!client.connected()){
        reconnect();   
    }
}
