#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 22
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// WiFi
const char *ssid = "4G-MIFI-549"; // Enter your Wi-Fi name
const char *password = "1234567890";  // Enter Wi-Fi password

// MQTT Broker
// const char *mqtt_broker = "192.168.1.167";
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32/led";
// const char *mqtt_username = "katvin";
// const char *mqtt_password = "katvin";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port); // Mengatur detail broker target  yang digunakan
  mqttClient.setCallback(callback);           // jika kita ingin menerima pesan untuk langganan yang dibuat oleh klien
}

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    dht.begin();

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    setupMQTT();
    client.publish(topic, "Hi, I'm ESP32 ^^");
    client.subscribe(topic);
    
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
      }      
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();

    if (!mqttClient.connected()){
    reconnect();   
  }
  else{
    float temperature = dht.readTemperature();
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);  
    Serial.print("Temperature: ");
    Serial.println(tempString);
    mqttClient.publish("emqx/esp32/temperature", tempString);
  
    float humidity = dht.readHumidity();;
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    mqttClient.publish("emqx/esp32/humidity", humString);
    delay(2000);
}
}
