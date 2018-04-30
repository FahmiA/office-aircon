#include "ota.hpp"

#include <ArduinoOTA.h>

void ota_onStart();
void ota_onProgress(unsigned int progress, unsigned int total);
void ota_onEnd();
void ota_onError(ota_error_t error);

void ota_setup(Config* config) {
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.setHostname(config_getName(config));

    ArduinoOTA.onStart(ota_onStart);
    ArduinoOTA.onEnd(ota_onEnd);
    ArduinoOTA.onProgress(ota_onProgress);
    ArduinoOTA.onError(ota_onError);

    ArduinoOTA.begin();
}

void ota_loop() {
    ArduinoOTA.handle();
}

void ota_onStart() {
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    if (ArduinoOTA.getCommand() == U_FLASH) {
        Serial.println("Start updating Sketch");
    } else { // U_SPIFFS
        Serial.println("Start updating File System");
    }
}

void ota_onProgress(unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void ota_onEnd() {
    Serial.println("\nEnd");
}

void ota_onError(ota_error_t error) {
    Serial.printf("\nError[%u]: ", error);

    if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
    }
}
