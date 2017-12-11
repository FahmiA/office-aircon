#ifndef PUBSUB
#define PUBSUB

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient pubsubClient;

struct PubSubSetting {
    const char* publicChannel;
    const char* privateChannel;
    const char* username;
    const char* password;
};

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port, MQTT_CALLBACK_SIGNATURE);
void pubsub_loop(PubSubSetting *setting);
void pubsub_reconnect(PubSubSetting *setting);


#endif // PUBSUB
