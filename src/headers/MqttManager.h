#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MqttManager {
private:
    WiFiClient espClient;
    PubSubClient client;
    
    const char* mqtt_server;
    int mqtt_port;
    const char* base_topic;

    void reconnect();

public:
    MqttManager(const char* server, int port, const char* topic);
    
    void begin();
    void loop();
    bool publish(const char* topic, const char* payload);
    bool isConnected();
};

#endif