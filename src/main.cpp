#include "Arduino.h"
#include "ir.send/ir.send.hpp"

//#ifndef LED_BUILTIN
    //#define LED_BUILTIN 13
//#endif

//#define IRLED_PIN LED_BUILTIN

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN 16

struct IRSettingCfg settings;

void setup() {
    Serial.begin(9600);

    pinMode(IRLED_PIN, OUTPUT);

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

