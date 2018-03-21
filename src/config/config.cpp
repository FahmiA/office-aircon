#include "config.hpp"

#include <Arduino.h>
#include <EEPROM.h>

/*
 * Notes:
 *  - ESP8266 only has 512 bytes of flash memory (not EEPROM but we still use the EEPROM Arduino lib)
 */
#define ESP8266_FLASH_SIZE 512 // bytes

Config* config_load() {
    EEPROM.begin(ESP8266_FLASH_SIZE);

    if (EEPROM.read(0) != CONFIG_VERSION) {
        return NULL;
    }

    Config *config = new Config();
    EEPROM.get(1, *config);

    return config;
}

void config_set(Config* config) {
    EEPROM.begin(ESP8266_FLASH_SIZE);

    EEPROM.put(0, 1);
    EEPROM.put(1, *config);

    EEPROM.commit();
}

void config_dump() {
    EEPROM.begin(ESP8266_FLASH_SIZE);

    int length = EEPROM.length();
    Serial.printf("Dumping flash. %d bytes\n", length);

    byte value;
    for(int i = 0; i < 150; i++) { // Keep it short
        value = EEPROM.read(i);

        Serial.print(i);
        Serial.print("\t");
        Serial.print((char)value);
        Serial.println();
    }
}

void config_print(Config *config) {
    Serial.println("Config:");
    Serial.printf("WiFi SSID\t%s\n", config->wifiSSID);
    Serial.printf("WiFi Password\t%s\n", config->wifiPassword);
    Serial.printf("MQTT Server\t%s:%d\n", config->mqttServer, config->mqttPort);
    Serial.printf("MQTT Auth\t%s/%s\n", config->mqttUsername, config->mqttPassword);
    Serial.printf("MQTT Client \t%s\n", config->mqttClientID);
}
