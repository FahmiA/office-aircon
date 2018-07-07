#include "pubsub.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "../config/config.hpp"

#define RECONNECT_DELAY_MS 5000 // 5 seconds

PubSubClient pubsubClient;

unsigned long lastReconnectMS = 0;

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE) {
    Serial.printf("MQTT: %s:%d\n", server, port);

    pubsubClient = PubSubClient(*wifiClient);
    pubsubClient.setServer(server, port);
    pubsubClient.setCallback(callback);
}

void pubsub_loop(Config *config, PubSubSetting *setting) {
    if (!pubsubClient.connected() && millis() - lastReconnectMS > RECONNECT_DELAY_MS) {
        pubsub_reconnect(config, setting);
        lastReconnectMS = millis();
    }
    pubsubClient.loop();
}

bool pubsub_isConnected() {
    return pubsubClient.connected();
}

bool pubsub_publish(const char* topic, const char* payload, bool retained) {
    return pubsubClient.publish(topic, payload, retained);
}

bool pubsub_publish(const char* topic, int payload, bool retained) {
    char buffer[6];
    itoa(payload, buffer, 10);
    return pubsub_publish(topic, buffer, retained);
}

void pubsub_reconnect(Config *config, PubSubSetting *setting) {
    Serial.print("Attempting MQTT connection...");

    char* clientId = config_getName(config);

    if (pubsubClient.connect(clientId, setting->username, setting->password)) {
        Serial.println("connected");

        pubsubClient.subscribe(setting->channelModel);
        pubsubClient.subscribe(setting->channelPower);
        pubsubClient.subscribe(setting->channelMode);
        pubsubClient.subscribe(setting->channelTemp);
        pubsubClient.subscribe(setting->channelFanSpeed);
        pubsubClient.subscribe(setting->channelFanVert);
        pubsubClient.subscribe(setting->channelFanHorz);
    } else {
        Serial.printf("failed, rc=%d Retry in 5 seconds\n", pubsubClient.state());
    }

    free(clientId);
}

