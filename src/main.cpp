#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "network/wifi.hpp"
#include "network/pubsub.hpp"
#include "ir.send/ir.send.hpp"

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN 16

#define MQTT_CHANNEL_PUBLIC "climate/aircon"
#define MQTT_CHANNEL_PRIVATE "climate/aircon/1"

IRSettingCfg* parseIRSettingsJSON(byte* payload);
void onIRRequest(char* topic, byte* payload, unsigned int length);

PubSubSetting pubsubSetting = {MQTT_CHANNEL_PUBLIC, MQTT_CHANNEL_PRIVATE, MQTT_USERNAME, MQTT_PASSWORD};

WiFiClient espClient;

void setup() {
    Serial.begin(9600);
    pinMode(IRLED_PIN, OUTPUT);
    randomSeed(micros());

    wifi_setup(WIFI_SSID, WIFI_PASSWORD);
    pubsub_setup(&espClient, MQTT_SERVER, MQTT_PORT, onIRRequest);
}

void loop() {
    pubsub_loop(&pubsubSetting);
}

void onIRRequest(char* topic, byte* payload, unsigned int length) {
    Serial.println("IR request received");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    IRSettingCfg *settings = parseIRSettingsJSON(payload);

    Serial.println("Sending IR sequence...");
    irSend(IRLED_PIN, settings);
    Serial.println("\tDone");

    free(settings);
}

IRSettingCfg* parseIRSettingsJSON(byte* payload) {
    // Should be large enough
    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
        Serial.println("\tparseRequestJSON() failed");
        return NULL;
    }

    const bool power = root["power"];
    const char* mode = root["mode"];
    const uint8_t temp = (uint8_t)root["temp"];
    const char* fanSpeed = root["fanSpeed"];
    const char* fanVert = root["fanVert"];
    const char* fanHorz = root["fanHorz"];

    Serial.println("Parsed IRSettings JSON:");
    Serial.printf("\tPower: %d\n", power);
    Serial.printf("\tMode: %s\n", mode);
    Serial.printf("\tTemp: %d\n", temp);
    Serial.printf("\tfanSpeed: %s\n", fanSpeed);
    Serial.printf("\tfanVert: %s\n", fanVert);
    Serial.printf("\tfanHorz: %s\n", fanHorz);

    IRSettingCfg *settings = new IRSettingCfg;

    Serial.println("Parsing Power");
    if(power) {
        settings->power = PowerOn;
    } else {
        settings->power = PowerOff;
    }

    Serial.println("Parsing Mode");
    switch(mode[0]) {
        case 'a': settings->mode = ModeAuto; break;
        case 'h': settings->mode = ModeHeat; break;
        case 'c': settings->mode = ModeCool; break;
        case 'd': settings->mode = ModeDry; break;
        case 'f': settings->mode = ModeFan; break;
        default: Serial.println("Missing mode"); return NULL;
    }

    Serial.println("Parsing temp");
    if(temp) {
        IRSetting settingTemp = {"Temp", temp};
        settings->temp = settingTemp;
    } else {
        Serial.println("Missing temp");
        return NULL;
    }

    Serial.println("Parsing fan speed");
    switch(fanSpeed[0]) {
        case 'a': settings->fanSpeed = FanSpeedAuto; break;
        case 'l': settings->fanSpeed = FanSpeedLow; break;
        case 'm': settings->fanSpeed = FanSpeedMed; break;
        case 'h': settings->fanSpeed = FanSpeedHigh; break;
        default: Serial.println("Missing fan speed"); return NULL;
    }

    Serial.println("Parsing fan dir vert");
    switch(fanVert[0]) {
        case 'a': settings->fanDirVert = FanVertAuto; break;
        case 'u': settings->fanDirVert = FanVertUp; break;
        case 'm':
                  switch(fanVert[1]) {
                      case 'u': settings->fanDirVert = FanVertMiddleUp; break;
                      case 'd': settings->fanDirVert = FanVertMiddleDown; break;
                      default:  settings->fanDirVert = FanVertMiddle; break;
                  }
                  break;
        case 'd': settings->fanDirVert = FanVertDown; break;
        case 's': settings->fanDirVert = FanVertSwing; break;
        default: Serial.println("Missing fan vert"); return NULL;
    }

    Serial.println("Parsing fan dir horz");
    switch(fanHorz[0]) {
        case 'a': settings->fanDirHorz = FanVertAuto; break;
        case 'l':
                  switch(fanHorz[1]) {
                      case 'r': settings->fanDirHorz = FanHorzLeftRight; break;
                      default:  settings->fanDirHorz = FanHorzLeft; break;
                  }
                  break;
        case 'm':
                  switch(fanHorz[1]) {
                      case 'l': settings->fanDirHorz = FanHorzMiddleLeft; break;
                      case 'r': settings->fanDirHorz = FanHorzMiddleRight; break;
                      default:  settings->fanDirHorz = FanHorzMiddle; break;
                  }
                  break;
        case 'r': settings->fanDirHorz = FanHorzRight; break;
        case 's': settings->fanDirHorz = FanHorzSwing; break;
        default: Serial.println("Missing fan horz"); return NULL;
    }

    return settings;
}
