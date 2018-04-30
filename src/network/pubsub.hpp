#ifndef PUBSUB
#define PUBSUB

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "../config/config.hpp"

extern PubSubClient pubsubClient;

struct PubSubSetting {
    char* username;
    char* password;
    char* channelPower;
    char* channelMode;
    char* channelTemp;
    char* channelFanSpeed;
    char* channelFanVert;
    char* channelFanHorz;
    char* channelSenseTemperature;
    char* channelSenseTemperatureFeel;
    char* channelSenseHumidity;
};

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE);
void pubsub_loop(Config *config, PubSubSetting *setting);
bool pubsub_isConnected();
bool pubsub_publish(const char* topic, const char* payload, bool retained = false);
bool pubsub_publish(const char* topic, int payload, bool retained = false);
void pubsub_reconnect(Config *config, PubSubSetting *setting);


#endif // PUBSUB
