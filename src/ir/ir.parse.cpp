#include "ir.parse.hpp"

bool irParseBool(byte* payload) {
    byte value = payload[0];
    return value == 1 || value == '1' || value == 't' || value == 'T' || value == 'y' || value == 'Y';
}

uint8_t irParseInt(byte* payload) {
    return atoi((char*)payload);
}

char* irParseString(byte* payload) {
    return (char*) payload;
}

IRSetting* irParsePower(bool value) {
    if (value) {
        return &PowerOn;
    }

    return &PowerOff;
}

IRSetting* irParseMode(const char* value) {
    switch(value[0]) {
        case 'a': return &ModeAuto;
        case 'h': return &ModeHeat;
        case 'c': return &ModeCool;
        case 'd': return &ModeDry;
        case 'f': return &ModeFan;
        default: return NULL;
    }
}

IRSetting* irParseTemp(uint8_t value) {
    IRSetting *settingTemp = new IRSetting();
    settingTemp->name = "Temp";
    settingTemp->value = value;
    return settingTemp;
}

IRSetting* irParseFanSpeed(const char* value) {
    switch(value[0]) {
        case 'a': return &FanSpeedAuto;
        case 'l': return &FanSpeedLow;
        case 'm': return &FanSpeedMed;
        case 'h': return &FanSpeedHigh;
        default: return NULL;
    }
}

IRSetting* irParseFanDirVert(const char* value) {
    switch(value[0]) {
        case 'a': return &FanVertAuto;
        case 'u': return &FanVertUp;
        case 'm':
                  switch(value[1]) {
                      case 'u':  return &FanVertMiddleUp;
                      case 'd':  return &FanVertMiddleDown;
                      default: return &FanVertMiddle;
                  }
                  break;
        case 'd': return &FanVertDown;
        case 's': return &FanVertSwing;
        default: return NULL;
    }
}

IRSetting* irParseFanDirHorz(const char* value) {
    switch(value[0]) {
        case 'a': return &FanVertAuto;
        case 'l':
                  switch(value[1]) {
                      case 'r': return &FanHorzLeftRight;
                      default: return &FanHorzLeft;
                  }
        case 'm':
                  switch(value[1]) {
                      case 'l': return &FanHorzMiddleLeft;
                      case 'r': return &FanHorzMiddleRight;
                      default: return &FanHorzMiddle;
                  }
                  break;
        case 'r': return &FanHorzRight;
        case 's': return &FanHorzSwing;
        default: return NULL;
    }
}

