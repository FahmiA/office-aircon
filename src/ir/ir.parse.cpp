#include "ir.parse.hpp"

#include "ir.target.hpp"

bool irParseBool(byte* payload) {
    byte value = payload[0];
    return value == 1 || value == '1' || value == 't' || value == 'T' || value == 'y' || value == 'Y';
}

uint8_t irParseInt(byte* payload) {
    return atoi((char*) payload);
}

char* irParseString(byte* payload) {
    return (char*) payload;
}

uint8_t irParseModel(const char* value) {
    //return irTargetGetId(value);
    return 0;
}

IRPower irParsePower(bool value) {
    if (value) {
        return IRPower::On;
    }

    return IRPower::Off;
}

IRMode irParseMode(const char* value) {
    switch(value[0]) {
        case 'a': return IRMode::Auto;
        case 'h': return IRMode::Heat;
        case 'c': return IRMode::Cool;
        case 'd': return IRMode::Dry;
        case 'f': return IRMode::Fan;
        default: return IRMode::Unknown;
    }
}

uint8_t irParseTemp(uint8_t value) {
    return value;
}

IRFanSpeed irParseFanSpeed(const char* value) {
    switch(value[0]) {
        case 'a': return IRFanSpeed::Auto;
        case 'l': return IRFanSpeed::Low;
        case 'm': return IRFanSpeed::Medium;
        case 'h': return IRFanSpeed::High;
        default: return IRFanSpeed::Unknown;
    }
}

IRFanVert irParseFanDirVert(const char* value) {
    switch(value[0]) {
        case 'a': return IRFanVert::Auto;
        case 'u': return IRFanVert::Up;
        case 'm':
                  switch(value[1]) {
                      case 'u':  return IRFanVert::MiddleUp;
                      case 'd':  return IRFanVert::MiddleDown;
                      default: return IRFanVert::Middle;
                  }
                  break;
        case 'd': return IRFanVert::Down;
        case 's': return IRFanVert::Swing;
        default: return IRFanVert::Unknown;
    }
}

IRFanHorz irParseFanDirHorz(const char* value) {
    switch(value[0]) {
        case 'a': return IRFanHorz::Auto;
        case 'l':
                  switch(value[1]) {
                      case 'r': return IRFanHorz::LeftRight;
                      default: return IRFanHorz::Left;
                  }
        case 'm':
                  switch(value[1]) {
                      case 'l': return IRFanHorz::MiddleLeft;
                      case 'r': return IRFanHorz::MiddleRight;
                      default: return IRFanHorz::Middle;
                  }
                  break;
        case 'r': return IRFanHorz::Right;
        case 's': return IRFanHorz::Swing;
        default: return IRFanHorz::Unknown;
    }
}

