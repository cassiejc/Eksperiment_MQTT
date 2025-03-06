#include <WiFi.h>
#include <PubSubClient.h>

// Konfigurasi WiFi
const char* ssid = "Telematics_AP";        // Ganti dengan SSID WiFi kamu
const char* password = "telematika"; // Ganti dengan password WiFi kamu

// Konfigurasi MQTT
const char* mqtt_server = "192.168.1.167"; // Bisa diganti dengan broker lain
const int mqtt_port = 1883;
const char* mqtt_user = "katvin";  // Ganti dengan username MQTT
const char* mqtt_pass = "katvin";  // Ganti dengan password MQTT
const char* mqtt_topic = "test";          // Topic MQTT untuk publish
const char* mqtt_topic_2 = "counter";     // Topic MQTT untuk publish
const char* mqtt_topic_3 = "led";         // Topic MQTT untuk subscribe

WiFiClient espClient;
PubSubClient client(espClient);

// Callback ketika menerima pesan MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Pesan diterima: ");
    String pesan;
    
    for (int i = 0; i < length; i++) {
        pesan += (char)payload[i]; // Menggabungkan payload jadi string
    }
    
    Serial.println(pesan);

    // Kontrol LED berdasarkan pesan
    if (pesan == "true") {
        digitalWrite(LED_BUILTIN, HIGH);  // LED menyala
    } else if (pesan == "false") {
        digitalWrite(LED_BUILTIN, LOW);   // LED mati
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT); // Atur LED sebagai output

    // Koneksi ke WiFi
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nTerhubung ke WiFi!");

    // Konfigurasi MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    reconnect();
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Menghubungkan ke MQTT...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) { // Gunakan username & password
            Serial.println("Terhubung!");
            client.subscribe(mqtt_topic_3);
        } else {
            Serial.print("Gagal, kode=");
            Serial.print(client.state());
            Serial.println(" Coba lagi dalam 5 detik...");
            delay(5000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Publish pesan ke MQTT
    String pesan = "test1";
    int randomNumber = random(1, 101); // Hasil antara 1 - 100
    String randomNumberStr = String(randomNumber); // Konversi int ke String

    client.publish(mqtt_topic, pesan.c_str());
    client.publish(mqtt_topic_2, randomNumberStr.c_str()); // Gunakan randomNumberStr

    Serial.println(pesan);
    Serial.println(randomNumber);

    delay(5000); // Kirim pesan setiap 5 detik
}
