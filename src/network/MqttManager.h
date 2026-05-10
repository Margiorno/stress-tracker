#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

class MqttManager {
private:
    WiFiClientSecure espClient;
    PubSubClient client;
    
    const char* mqtt_server;
    int mqtt_port;
    const char* base_topic;

    void reconnect();

public:
    MqttManager(const char* server, int port, const char* topic);
    
    void begin(const char* rootCA, const char* deviceCert, const char* deviceKey);
    void loop();
    bool publish(const char* topic, const char* payload);
    bool publishBinary(const char* topic, const uint8_t* payload, unsigned int length);
    bool isConnected();
};

#endif