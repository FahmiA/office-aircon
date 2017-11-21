#ifndef PUBSUB
#define PUBSUB

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient pubsubClient;

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port);
void pubsub_loop();
void pubsub_reconnect();
void pubsub_callback(char* topic, byte* payload, unsigned int length);


#endif // PUBSUB
