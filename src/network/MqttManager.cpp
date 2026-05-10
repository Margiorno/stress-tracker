#include "MqttManager.h"

MqttManager::MqttManager(const char* server, int port, const char* topic) {
    mqtt_server = server;
    mqtt_port = port;
    base_topic = topic;
    
    client.setClient(espClient);

    client.setBufferSize(2048);
}

void MqttManager::begin(const char* rootCA, const char* deviceCert, const char* deviceKey) {
    espClient.setCACert(rootCA);
    espClient.setCertificate(deviceCert);
    espClient.setPrivateKey(deviceKey);
    
    client.setServer(mqtt_server, mqtt_port);
}

void MqttManager::reconnect() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("Connected to MQTT!");
            // client.subscribe(base_topic); 
        } else {
            Serial.print("Connection failed, rc=");
            Serial.print(client.state());

            char err_buf[100];
            if (espClient.lastError(err_buf, sizeof(err_buf)) < 0) {
                Serial.print(" | TLS Error: ");
                Serial.print(err_buf);
            }

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

bool MqttManager::publishBinary(const char* topic, const uint8_t* payload, unsigned int length) {
    if (client.connected()) {
        return client.publish(topic, payload, length);
    }
    return false;
}

bool MqttManager::isConnected() {
    return client.connected();
}