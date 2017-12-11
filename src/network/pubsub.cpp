#include "pubsub.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

PubSubClient pubsubClient;

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE) {
    pubsubClient = PubSubClient(*wifiClient);

    Serial.println(server);
    Serial.println(port);
    pubsubClient.setServer(server, port);
    pubsubClient.setCallback(callback);
}

void pubsub_loop(PubSubSetting *setting) {
    if (!pubsubClient.connected()) {
        pubsub_reconnect(setting);
    }
    pubsubClient.loop();
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
            // Once connected, publish an announcement...
            //pubsubClient.publish("outTopic", "hello world");
            // ... and resubscribe
            pubsubClient.subscribe(setting->channelPrivateAll);
            pubsubClient.subscribe(setting->channelPrivatePower);
            pubsubClient.subscribe(setting->channelPrivateMode);
            pubsubClient.subscribe(setting->channelPrivateTemp);
            pubsubClient.subscribe(setting->channelPrivateFanSpeed);
            pubsubClient.subscribe(setting->channelPrivateFanVert);
            pubsubClient.subscribe(setting->channelPrivateFanHorz);
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubsubClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

