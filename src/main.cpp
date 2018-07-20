#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#include "config/config.hpp"
#include "network/wifi.hpp"
#include "network/pubsub.hpp"
#include "ir/ir.target.hpp"
#include "ir/ir.target.custom.hpp"
#include "ir/ir.parse.hpp"
#include "ota/ota.hpp"

// LoLin NodeMCU pins (https://github.com/esp8266/Arduino/issues/584)
#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN D5

#define DHT_TYPE DHT11
#define DHT_PIN D4

#define IR_EVENT_DELAY_MS 2000 // 2 seconds
#define TEMP_EVENT_DELAY_MS 300000 // 5 minutes

PubSubSetting* makePubSubSetting(Config* config);

void onRequest(char* topic, byte* payload, unsigned int length);
void sendIRSequence();
void takeTempReading();

IRSettingCfg* parseIRSettingModel(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, bool value);
IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingTemp(IRSettingCfg *settings, uint8_t value);
IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, const char* value);
IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, const char* value);

Config* config;
PubSubSetting* pubsubSetting;

WiFiClient espClient; // Use 'WiFiClientSecure' for SSL connection
IRSettingCfg lastSettings { new IRCustomMitsubishiTarget(), IRPower::Off, IRMode::Auto, (uint8_t) 21, IRFanSpeed::Auto, IRFanVert::Auto, IRFanHorz::Auto };
unsigned long lastIREventMS = 0;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastTempEventMS = 0;

void setup() {
    Serial.begin(9600);

    if (FIRST_TIME_SETUP == true) {
        Serial.println("Storing first-time configuration");
        config = config_loadFromEnvironment();
        config_set(config);
        config_print(config);
    } else {
        config = config_load();
    }

    if (config == NULL) {
        Serial.println("No config found");
    }

    pubsubSetting = makePubSubSetting(config);

    pinMode(IRLED_PIN, OUTPUT);
    randomSeed(micros());

    wifi_setup(config->wifiSSID, config->wifiPassword);
    ota_setup(config);
    pubsub_setup(&espClient, config->mqttServer, config->mqttPort, onRequest);

    dht.begin();

    lastTempEventMS = millis();
}

void loop() {
    if (config == NULL) {
        return;
    }

    ota_loop();
    pubsub_loop(config, pubsubSetting);

    unsigned long ms = millis();
    if(lastIREventMS > 0 && ms - lastIREventMS > IR_EVENT_DELAY_MS) {
        lastIREventMS = 0;
        sendIRSequence();
    } 

    if(ms - lastTempEventMS > TEMP_EVENT_DELAY_MS) {
        lastTempEventMS = ms;
        takeTempReading();
    }
}

PubSubSetting* makePubSubSetting(Config* config) {
    PubSubSetting* setting = new PubSubSetting();
    setting->username = config->mqttUsername;
    setting->password = config->mqttPassword;
    asprintf(&setting->channelModel, "aircon/%s/model", config->mqttClientID);
    asprintf(&setting->channelPower, "aircon/%s/power", config->mqttClientID);
    asprintf(&setting->channelMode, "aircon/%s/mode", config->mqttClientID);
    asprintf(&setting->channelTemp, "aircon/%s/temp", config->mqttClientID);
    asprintf(&setting->channelFanSpeed, "aircon/%s/fan-speed", config->mqttClientID);
    asprintf(&setting->channelFanVert, "aircon/%s/fan-vert", config->mqttClientID);
    asprintf(&setting->channelFanHorz, "aircon/%s/fan-horz", config->mqttClientID);
    asprintf(&setting->channelSenseTemperature, "aircon/%s/temperature", config->mqttClientID);
    asprintf(&setting->channelSenseTemperatureFeel, "aircon/%s/temperature-feel", config->mqttClientID);
    asprintf(&setting->channelSenseHumidity, "aircon/%s/humidity", config->mqttClientID);

    return setting;
}

void onRequest(char* topic, byte* payload, unsigned int length) {
    Serial.printf("IR request received: %s\n\t", topic);
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();

    IRSettingCfg *settings = NULL;
    if(strcmp(topic, pubsubSetting->channelPower) == 0) {
        settings = parseIRSettingModel(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting->channelPower) == 0) {
        settings = parseIRSettingPower(&lastSettings, irParseBool(payload));
    } else if(strcmp(topic, pubsubSetting->channelMode) == 0) {
        settings = parseIRSettingMode(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting->channelTemp) == 0) {
        settings = parseIRSettingTemp(&lastSettings, irParseInt(payload));
    } else if(strcmp(topic, pubsubSetting->channelFanSpeed) == 0) {
        settings = parseIRSettingFanSpeed(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting->channelFanVert) == 0) {
        settings = parseIRSettingFanDirVert(&lastSettings, irParseString(payload));
    } else if(strcmp(topic, pubsubSetting->channelFanHorz) == 0) {
        settings = parseIRSettingFanDirHorz(&lastSettings, irParseString(payload));
    }

    if(settings == NULL) {
        return;
    }

    lastSettings = *settings;

    lastIREventMS = millis();
}

void sendIRSequence() {
    Serial.print("Sending IR sequence... ");
    lastSettings.model->send(IRLED_PIN, &lastSettings);
    Serial.println("Done");
}

void takeTempReading() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C\n");

    pubsub_publish(pubsubSetting->channelSenseTemperature, t, true);
    pubsub_publish(pubsubSetting->channelSenseTemperatureFeel, hic, true);
    pubsub_publish(pubsubSetting->channelSenseHumidity, h, true);
}

IRSettingCfg* parseIRSettingModel(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing model");
    IRTarget *target = irParseModel(value);
    if (target == NULL) {
        Serial.println("\tMissing target");
        return NULL;
    }

    if(strcmp(settings->model->getName(), target->getName())) {
        Serial.println("\tSame model");
        return NULL;
    }

    delete settings->model;
    settings->model = target;
    return settings;
}

IRSettingCfg* parseIRSettingPower(IRSettingCfg *settings, bool value) {
    Serial.println("Parsing power");
    IRPower power = irParsePower(value);

    if(settings->power == power) {
        Serial.println("\tSame power");
        return NULL;
    }

    settings->power = power;
    return settings;
}

IRSettingCfg* parseIRSettingMode(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing mode");
    IRMode mode = irParseMode(value);
    if (mode == IRMode::Unknown) {
        Serial.println("\tMissing mode");
        return NULL;
    }

    if(settings->mode == mode) {
        Serial.println("\tSame mode");
        return NULL;
    }

    settings->mode = mode;
    return settings;
}

IRSettingCfg* parseIRSettingTemp(IRSettingCfg *settings, uint8_t value) {
    Serial.println("Parsing temperature");
    uint8_t temp = irParseTemp(value);
    if (temp < 0) { // TODO: Test min, max, and invalid temps
        Serial.println("\tMissing temperature");
        return NULL;
    }

    if(settings->temp == temp) {
        Serial.println("\tSame temp");
        return NULL;
    }

    settings->temp = temp;
    return settings;
}

IRSettingCfg* parseIRSettingFanSpeed(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan speed");
    IRFanSpeed fanSpeed = irParseFanSpeed(value);
    if (fanSpeed == IRFanSpeed::Unknown) {
        Serial.println("\tMissing fan speed");
        return NULL;
    }

    if(settings->fanSpeed == fanSpeed) {
        Serial.println("\tSame fan speed");
        return NULL;
    }

    settings->fanSpeed = fanSpeed;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirVert(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan dir vert");
    IRFanVert fanDirVert = irParseFanDirVert(value);
    if (fanDirVert == IRFanVert::Unknown) {
        Serial.println("\tMissing fan dir vert");
        return NULL;
    }

    if(settings->fanDirVert == fanDirVert) {
        Serial.println("\tSame fan dir vert");
        return NULL;
    }

    settings->fanDirVert = fanDirVert;
    return settings;
}

IRSettingCfg* parseIRSettingFanDirHorz(IRSettingCfg *settings, const char* value) {
    Serial.println("Parsing fan dir horz");
    IRFanHorz fanDirHorz = irParseFanDirHorz(value);
    if (fanDirHorz == IRFanHorz::Unknown) {
        Serial.println("\tMissing fan dir horz");
        return NULL;
    }

    if(settings->fanDirHorz == fanDirHorz) {
        Serial.println("\tSame fan dir horz");
        return NULL;
    }

    settings->fanDirHorz = fanDirHorz;
    return settings;
}

