#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "ir.send/ir.send.hpp"

//#ifndef LED_BUILTIN
//#define LED_BUILTIN 13
//#endif

//#define IRLED_PIN LED_BUILTIN

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN 16

const char* SSID = "SSID";
const char* PASSWORD = "PASSWORD";
const char* MQTT_SERVER = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

struct IRSettingCfg settings;

void setupWiFi() {
    Serial.println();
    Serial.printf("Connecting to: %s\n", SSID);

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
    }

    const bool power = root["power"];
    const char* mode = root["mode"];
    const uint8_t temp = (uint8_t)root["temp"];
    const char* fanSpeed = root["fanSpeed"];
    const char* fanVert = root["fanVert"];
    const char* fanHorz = root["fanHorz"];

    Serial.println("Parsed JSON:");
    Serial.printf("\tPower: %d\n", power);
    Serial.printf("\tMode: %s\n", mode);
    Serial.printf("\tTemp: %d\n", temp);
    Serial.printf("\tfanSpeed: %s\n", fanSpeed);
    Serial.printf("\tfanVert: %s\n", fanVert);
    Serial.printf("\tfanHorz: %s\n", fanHorz);

    if (power) {
        settings.power = PowerOn;
    } else {
        settings.power = PowerOff;
    }

    switch(mode[0]) {
        case 'a': settings.mode = ModeAuto; break;
        case 'h': settings.mode = ModeHeat; break;
        case 'c': settings.mode = ModeCool; break;
        case 'd': settings.mode = ModeDry; break;
        case 'f': settings.mode = ModeFan; break;
        default: Serial.println("Missing mode"); return;
    }

    if(temp) {
        IRSetting settingTemp = {"Temp", temp};
        settings.temp = settingTemp;
    } else {
        Serial.println("Missing temp");
        return;
    }

    switch(fanSpeed[0]) {
        case 'a': settings.fanSpeed = FanSpeedAuto; break;
        case 'l': settings.fanSpeed = FanSpeedLow; break;
        case 'm': settings.fanSpeed = FanSpeedMed; break;
        case 'h': settings.fanSpeed = FanSpeedHigh; break;
        default: Serial.println("Missing fan speed"); return;
    }

    switch(fanVert[0]) {
        case 'a': settings.fanDirVert = FanVertAuto; break;
        case 'u': settings.fanDirVert = FanVertUp; break;
        case 'm': 
            switch(fanVert[6]) {
                case 'u': settings.fanDirVert = FanVertMiddleUp; break;
                case 'd': settings.fanDirVert = FanVertMiddleDown; break;
                default:  settings.fanDirVert = FanVertMiddle; break;
            }
            break;
        case 'd': settings.fanDirVert = FanVertDown; break;
        case 's': settings.fanDirVert = FanVertSwing; break;
        default: Serial.println("Missing fan vert"); return;
    }

    switch(fanVert[0]) {
        case 'a': settings.fanDirHorz = FanVertAuto; break;
        case 'l': 
            switch(fanVert[4]) {
                case 'r': settings.fanDirHorz = FanHorzLeftRight; break;
                default:  settings.fanDirHorz = FanHorzLeft; break;
            }
            break;
        case 'm': 
            switch(fanVert[6]) {
                case 'l': settings.fanDirHorz = FanHorzMiddleLeft; break;
                case 'r': settings.fanDirHorz = FanHorzMiddleRight; break;
                default:  settings.fanDirHorz = FanHorzMiddle; break;
            }
            break;
        case 'r': settings.fanDirHorz = FanHorzRight; break;
        case 's': settings.fanDirHorz = FanHorzSwing; break;
        default: Serial.println("Missing fan horz"); return;
    }

    //Serial.println("Sending...");
    //irSend(IRLED_PIN, &settings);
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            //client.publish("outTopic", "hello world");
            // ... and resubscribe
            client.subscribe("_office_aircon_");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(IRLED_PIN, OUTPUT);

    setupWiFi();

    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);

    randomSeed(micros());

    settings.power = PowerOn;
    settings.mode = ModeCool;
    settings.fanSpeed = FanSpeedHigh;
    settings.fanDirVert = FanVertDown;
    settings.fanDirHorz = FanHorzSwing;

    IRSetting temp = {"Temp", 19};
    settings.temp = temp;

    Serial.printf("Settings\n");
    Serial.printf("\tPower:\t\t%s\n", settings.power.name);
    Serial.printf("\tMode:\t\t%s\n", settings.mode.name);
    Serial.printf("\tTemp:\t\t%s\t\t%d\n", settings.temp.name, settings.temp.value);
    Serial.printf("\tFan Speed:\t%s\n", settings.fanSpeed.name);
    Serial.printf("\tFan Vert:\t%s\n", settings.fanDirVert.name);
    Serial.printf("\tFan Horz:\t%s\n", settings.fanDirHorz.name);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    //delay(5000);
    //Serial.println("Sending...");
    //irSend(IRLED_PIN, &settings);
}

