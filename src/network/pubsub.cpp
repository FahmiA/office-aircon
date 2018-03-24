#include "pubsub.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

PubSubClient pubsubClient;

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE) {
    Serial.printf("MQTT: %s:%d\n", server, port);

    pubsubClient = PubSubClient(*wifiClient);
    pubsubClient.setServer(server, port);
    pubsubClient.setCallback(callback);
}

void pubsub_loop(PubSubSetting *setting) {
    if (!pubsubClient.connected()) {
        pubsub_reconnect(setting);
    }
    pubsubClient.loop();
}

bool isConnected() {
    return pubsubClient.connected();
}

bool pubsub_publish(const char* topic, const char* payload, bool retained) {
    return pubsubClient.publish(topic, payload, retained);
}

bool pubsub_publish(const char* topic, int payload, bool retained) {
    char buffer[6];
    itoa(payload, buffer, 10);
    pubsub_publish(topic, buffer, retained);
}

void pubsub_reconnect(PubSubSetting *setting) {
    // Loop until we're reconnected
    while (!pubsubClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (pubsubClient.connect(clientId.c_str(), setting->username, setting->password)) {
            Serial.println("connected");
            // resubscribe
            pubsubClient.subscribe(setting->channelPower);
            pubsubClient.subscribe(setting->channelMode);
            pubsubClient.subscribe(setting->channelTemp);
            pubsubClient.subscribe(setting->channelFanSpeed);
            pubsubClient.subscribe(setting->channelFanVert);
            pubsubClient.subscribe(setting->channelFanHorz);
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubsubClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

