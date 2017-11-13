#include "Arduino.h"
#include "ir.send/ir.send.hpp"

#ifndef LED_BUILTIN
    #define LED_BUILTIN 13
#endif

#define IRLED_PIN LED_BUILTIN

struct IRSettingCfg settings;

void setup() {
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("Start");
    printf("Start\n");

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
    delay(5000);

    Serial.println("Sending...");
    irSend(IRLED_PIN, &settings);
}

