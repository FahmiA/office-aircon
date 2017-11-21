#include "pubsub.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

PubSubClient pubsubClient;

void pubsub_setup(WiFiClient *wifiClient, const char* server, int port) {
    pubsubClient = PubSubClient(*wifiClient);

    Serial.println(server);
    Serial.println(port);
    pubsubClient.setServer(server, port);
    pubsubClient.setCallback(pubsub_callback);
}

void pubsub_loop() {
    if (!pubsubClient.connected()) {
        pubsub_reconnect();
    }
    pubsubClient.loop();
}

void pubsub_reconnect() {
    // Loop until we're reconnected
    while (!pubsubClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (pubsubClient.connect(clientId.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            //pubsubClient.publish("outTopic", "hello world");
            // ... and resubscribe
            pubsubClient.subscribe("_office_aircon_");
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubsubClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void pubsub_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    //StaticJsonBuffer<200> jsonBuffer;
    //JsonObject& root = jsonBuffer.parseObject(payload);
    //if (!root.success()) {
    //    Serial.println("parseObject() failed");
    //    return;
    //}

    //const bool power = root["power"];
    //const char* mode = root["mode"];
    //const uint8_t temp = (uint8_t)root["temp"];
    //const char* fanSpeed = root["fanSpeed"];
    //const char* fanVert = root["fanVert"];
    //const char* fanHorz = root["fanHorz"];

    //Serial.println("Parsed JSON:");
    //Serial.printf("\tPower: %d\n", power);
    //Serial.printf("\tMode: %s\n", mode);
    //Serial.printf("\tTemp: %d\n", temp);
    //Serial.printf("\tfanSpeed: %s\n", fanSpeed);
    //Serial.printf("\tfanVert: %s\n", fanVert);
    //Serial.printf("\tfanHorz: %s\n", fanHorz);

    //if (power) {
    //    settings.power = PowerOn;
    //} else {
    //    settings.power = PowerOff;
    //}

    //switch(mode[0]) {
    //    case 'a': settings.mode = ModeAuto; break;
    //    case 'h': settings.mode = ModeHeat; break;
    //    case 'c': settings.mode = ModeCool; break;
    //    case 'd': settings.mode = ModeDry; break;
    //    case 'f': settings.mode = ModeFan; break;
    //    default: Serial.println("Missing mode"); return;
    //}

    //if(temp) {
    //    IRSetting settingTemp = {"Temp", temp};
    //    settings.temp = settingTemp;
    //} else {
    //    Serial.println("Missing temp");
    //    return;
    //}

    //switch(fanSpeed[0]) {
    //    case 'a': settings.fanSpeed = FanSpeedAuto; break;
    //    case 'l': settings.fanSpeed = FanSpeedLow; break;
    //    case 'm': settings.fanSpeed = FanSpeedMed; break;
    //    case 'h': settings.fanSpeed = FanSpeedHigh; break;
    //    default: Serial.println("Missing fan speed"); return;
    //}

    //switch(fanVert[0]) {
    //    case 'a': settings.fanDirVert = FanVertAuto; break;
    //    case 'u': settings.fanDirVert = FanVertUp; break;
    //    case 'm': 
    //        switch(fanVert[6]) {
    //            case 'u': settings.fanDirVert = FanVertMiddleUp; break;
    //            case 'd': settings.fanDirVert = FanVertMiddleDown; break;
    //            default:  settings.fanDirVert = FanVertMiddle; break;
    //        }
    //        break;
    //    case 'd': settings.fanDirVert = FanVertDown; break;
    //    case 's': settings.fanDirVert = FanVertSwing; break;
    //    default: Serial.println("Missing fan vert"); return;
    //}

    //switch(fanVert[0]) {
    //    case 'a': settings.fanDirHorz = FanVertAuto; break;
    //    case 'l': 
    //        switch(fanVert[4]) {
    //            case 'r': settings.fanDirHorz = FanHorzLeftRight; break;
    //            default:  settings.fanDirHorz = FanHorzLeft; break;
    //        }
    //        break;
    //    case 'm': 
    //        switch(fanVert[6]) {
    //            case 'l': settings.fanDirHorz = FanHorzMiddleLeft; break;
    //            case 'r': settings.fanDirHorz = FanHorzMiddleRight; break;
    //            default:  settings.fanDirHorz = FanHorzMiddle; break;
    //        }
    //        break;
    //    case 'r': settings.fanDirHorz = FanHorzRight; break;
    //    case 's': settings.fanDirHorz = FanHorzSwing; break;
    //    default: Serial.println("Missing fan horz"); return;
    //}

    //Serial.println("Sending...");
    //irSend(IRLED_PIN, &settings);
}

