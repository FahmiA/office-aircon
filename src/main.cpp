#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "network/wifi.hpp"
#include "network/pubsub.hpp"
#include "ir.send/ir.send.hpp"
#include "ir.parse/ir.parse.hpp"

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN 16

IRSettingCfg* parseIRSettingAll(byte* payload);
IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, byte* payload);
IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, byte* payload);
IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, byte* payload);
IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, byte* payload);
IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, byte* payload);
void onIRRequest(char* topic, byte* payload, unsigned int length);

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
        settings = parseIRSettingPower(&lastSettings, payload);
    } else if(strcmp(topic, pubsubSetting.channelPrivateMode) == 0) {
        settings = parseIRSettingMode(&lastSettings, payload);
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanSpeed) == 0) {
        settings = parseIRSettingFanSpeed(&lastSettings, payload);
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanVert) == 0) {
        settings = parseIRSettingFanDirVert(&lastSettings, payload);
    } else if(strcmp(topic, pubsubSetting.channelPrivateFanHorz) == 0) {
        settings = parseIRSettingFanDirHorz(&lastSettings, payload);
    }

    if(settings == NULL) {
        return;
    }

    lastSettings = *settings;

    // should be large enough
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["power"] = settings->power.value;
    //root["mode"] = settings->mode.value;
    //root["temp"] = settings->power.value;
    //root["fanSpeed"] = settings->power.value;
    //root["fanVert"] = settings->power.value;
    //root["fanHorz"] = settings->power.value;
    //pubsub_publish(

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
    IRSetting *setting;

    Serial.println("Parsing power");
    setting = irParsePower(power);
    if (setting == NULL) {
        Serial.println("Missing power");
        delete settings;
        return NULL;
    }
    settings->power = *setting;

    Serial.println("Parsing mode");
    setting = irParseMode(mode);
    if (setting == NULL) {
        Serial.println("Missing mode");
        delete settings;
        return NULL;
    }
    settings->mode = *setting;

    Serial.println("Parsing temp");
    setting = irParseTemp(temp);
    if (setting == NULL) {
        Serial.println("Missing temp");
        delete settings;
        return NULL;
    }
    settings->temp = *setting;

    Serial.println("Parsing fan speed");
    setting = irParseFanSpeed(fanSpeed);
    if (setting == NULL) {
        Serial.println("Missing fan speed");
        delete settings;
        return NULL;
    }
    settings->fanSpeed = *setting;

    Serial.println("Parsing fan dir vert");
    setting = irParseFanDirVert(fanVert);
    if (setting == NULL) {
        Serial.println("Missing fan dir vert");
        delete settings;
        return NULL;
    }
    settings->fanDirVert = *setting;

    Serial.println("Parsing fan dir horz");
    setting = irParseFanDirHorz(fanHorz);
    if (setting == NULL) {
        Serial.println("Missing fan dir horz");
        delete settings;
        return NULL;
    }
    settings->fanDirHorz = *setting;

    return settings;
}

IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, byte* payload) {
    Serial.println("Parsing power");
    IRSetting *setting = irParsePower(payload[0] == 1 || payload[0] == '1' || payload[0] == 't');
    if (setting == NULL) {
        Serial.println("Missing power");
        return NULL;
    }

    settings->power = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, byte* payload) {
    Serial.println("Parsing mode");
    IRSetting *setting = irParseMode((char*)payload);
    if (setting == NULL) {
        Serial.println("Missing mode");
        return NULL;
    }

    settings->mode = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, byte* payload) {
    Serial.println("Parsing fan speed");
    IRSetting *setting = irParseFanSpeed((char*)payload);
    if (setting == NULL) {
        Serial.println("Missing fan speed");
        return NULL;
    }

    settings->fanSpeed = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, byte* payload) {
    Serial.println("Parsing fan dir vert");
    IRSetting *setting = irParseFanDirVert((char*)payload);
    if (setting == NULL) {
        Serial.println("Missing fan dir vert");
        return NULL;
    }

    settings->fanDirVert = *setting;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, byte* payload) {
    Serial.println("Parsing fan dir horz");
    IRSetting *setting = irParseFanDirHorz((char*)payload);
    if (setting == NULL) {
        Serial.println("Missing fan dir horz");
        return NULL;
    }

    settings->fanDirHorz = *setting;
    return settings;
}

