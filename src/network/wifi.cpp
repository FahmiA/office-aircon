#include "wifi.hpp"

#include <Arduino.h>
#include <ESP8266WiFi.h>

void wifi_setup(const char* ssid, const char* password) {
    Serial.printf("Connecting to: %s\n", ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}
