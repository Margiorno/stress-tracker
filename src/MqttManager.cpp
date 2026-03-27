#include "MqttManager.h"

MqttManager::MqttManager(const char* server, int port, const char* topic) {
    mqtt_server = server;
    mqtt_port = port;
    base_topic = topic;
    
    client.setClient(espClient);
}

void MqttManager::begin() {
    client.setServer(mqtt_server, mqtt_port);
}

void MqttManager::reconnect() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("Connected to MQTT!");
            client.subscribe(base_topic); 
        } else {
            Serial.print("Connection failed, rc=");
            Serial.print(client.state());
            Serial.println(" will attempt reconnect in 5 seconds");
            delay(5000);
        }
    }
}

void MqttManager::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

bool MqttManager::publish(const char* topic, const char* payload) {
    if (client.connected()) {
        return client.publish(topic, payload);
    }
    return false;
}

bool MqttManager::isConnected() {
    return client.connected();
}