#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "network/wifi.hpp"
#include "network/pubsub.hpp"
#include "ir/ir.send.hpp"
#include "ir/ir.parse.hpp"

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN 16

void onIRRequest(char* topic, byte* payload, unsigned int length);

IRSettingCfg* parseIRSettingAll(byte* payload);
IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, bool value);
IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingTemp(IRSettingCfg *settings, uint8_t value);
IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, const char* value);

PubSubSetting pubsubSetting = {
    MQTT_USERNAME
    MQTT_PASSWORD, // Issue with password from private.sh
    "aircon/" MQTT_CLIENT_ID,
    "aircon/" MQTT_CLIENT_ID "/power",
    "aircon/" MQTT_CLIENT_ID "/mode",
    "aircon/" MQTT_CLIENT_ID "/temp",
    "aircon/" MQTT_CLIENT_ID "/fan-speed",
    "aircon/" MQTT_CLIENT_ID "/fan-vert",
    "aircon/" MQTT_CLIENT_ID "/fan-horz"
};

WiFiClient espClient;
IRSettingCfg lastSettings { PowerOff, ModeAuto, {"Temp", (uint8_t)21}, FanSpeedAuto, FanVertAuto, FanHorzAuto };

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
    Serial.print("IR request received: ");
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    IRSettingCfg *settings = NULL;
    if(strcmp(topic, pubsubSetting.channelPrivateAll) == 0) {
        settings = parseIRSettingAll(payload);
    } else if(strcmp(topic, pubsubSetting.channelPrivatePower) == 0) {
        settings = parseIRSettingPower(&lastSettings, irParseBool(payload));
    } else if(strcmp(topic, pubsubSetting.channelPrivateMode) == 0) {
        settings = parseIRSettingMode(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting.channelPrivateTemp) == 0) {
        settings = parseIRSettingTemp(&lastSettings, irParseInt(payload));
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanSpeed) == 0) {
        settings = parseIRSettingFanSpeed(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanVert) == 0) {
        settings = parseIRSettingFanDirVert(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanHorz) == 0) {
        settings = parseIRSettingFanDirHorz(&lastSettings, irParseString(payload));
    }

    if(settings == NULL) {
        return;
    }

    lastSettings = *settings;

    Serial.println("Sending IR sequence...");
    irSend(IRLED_PIN, settings);
    Serial.println("\tDone");
}

IRSettingCfg* parseIRSettingAll(byte* payload) {
    // should be large enough
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

    IRSettingCfg *settings = new IRSettingCfg();

    settings = parseIRSettingPower(settings, power);
    settings = settings != NULL ? parseIRSettingMode(settings, mode) : NULL;
    settings = settings != NULL ? parseIRSettingTemp(settings, temp) : NULL;
    settings = settings != NULL ? parseIRSettingFanSpeed(settings, fanSpeed) : NULL;
    settings = settings != NULL ? parseIRSettingFanDirVert(settings, fanVert) : NULL;
    settings = settings != NULL ? parseIRSettingFanDirHorz(settings, fanHorz) : NULL;

    return settings;
}

IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, bool value) {
    Serial.println("Parsing power");
    IRSetting *setting = irParsePower(value);
    if (setting == NULL) {
        Serial.println("Missing power");
        return NULL;
    }

    settings->power = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing mode");
    IRSetting *setting = irParseMode(value);
    if (setting == NULL) {
        Serial.println("Missing mode");
        return NULL;
    }

    settings->mode = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingTemp(IRSettingCfg *settings, uint8_t value) {
    Serial.println("Parsing temperature");
    IRSetting *setting = irParseTemp(value);
    if (setting == NULL) {
        Serial.println("Missing temperature");
        return NULL;
    }

    settings->mode = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan speed");
    IRSetting *setting = irParseFanSpeed(value);
    if (setting == NULL) {
        Serial.println("Missing fan speed");
        return NULL;
    }

    settings->fanSpeed = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan dir vert");
    IRSetting *setting = irParseFanDirVert(value);
    if (setting == NULL) {
        Serial.println("Missing fan dir vert");
        return NULL;
    }

    settings->fanDirVert = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan dir horz");
    IRSetting *setting = irParseFanDirHorz(value);
    if (setting == NULL) {
        Serial.println("Missing fan dir horz");
        return NULL;
    }

    settings->fanDirHorz = *setting;
    return settings;
}

