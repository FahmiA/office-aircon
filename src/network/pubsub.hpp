#ifndef PUBSUB
#define PUBSUB

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient pubsubClient;

struct PubSubSetting {
    const char* username;
    const char* password;
    const char* channelPrivatePower;
    const char* channelPrivateMode;
    const char* channelPrivateTemp;
    const char* channelPrivateFanSpeed;
    const char* channelPrivateFanVert;
    const char* channelPrivateFanHorz;
    const char* channelSenseTemperature;
    const char* channelSenseTemperatureFeel;
    const char* channelSenseHumidity;
};

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE);
void pubsub_loop(PubSubSetting *setting);
bool isConnected();
bool pubsub_publish(const char* topic, const char* payload, bool retained = false);
bool pubsub_publish(const char* topic, int payload, bool retained = false);
void pubsub_reconnect(PubSubSetting *setting);


#endif // PUBSUB
