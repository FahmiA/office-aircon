#ifndef IR_SEND
#define IR_SEND

#include "Arduino.h"

#define IR_NUM_BYTES 14
#define IR_SIZOF_BYTE 8

struct IRSetting {
    const char* name;
    uint8_t value;
};

extern IRSetting PowerOff;
extern IRSetting PowerOn;

extern IRSetting ModeAuto;
extern IRSetting ModeHeat;
extern IRSetting ModeCool;
extern IRSetting ModeDry;
extern IRSetting ModeFan;

extern IRSetting FanSpeedAuto;
extern IRSetting FanSpeedLow;
extern IRSetting FanSpeedMed;
extern IRSetting FanSpeedHigh;

extern IRSetting FanVertAuto;
extern IRSetting FanVertUp;
extern IRSetting FanVertMiddleUp;
extern IRSetting FanVertMiddle;
extern IRSetting FanVertMiddleDown;
extern IRSetting FanVertDown;
extern IRSetting FanVertSwing;

extern IRSetting FanHorzAuto;
extern IRSetting FanHorzLeft;
extern IRSetting FanHorzMiddleLeft;
extern IRSetting FanHorzMiddle;
extern IRSetting FanHorzMiddleRight;
extern IRSetting FanHorzRight;
extern IRSetting FanHorzLeftRight;
extern IRSetting FanHorzSwing;

struct IRSettingCfg {
    IRSetting power;
    IRSetting mode;
    IRSetting temp;
    IRSetting fanSpeed;
    IRSetting fanDirVert;
    IRSetting fanDirHorz;
};

uint8_t irGetChecksum(uint8_t* bytes);
uint8_t* irGetBytes(struct IRSettingCfg *settings);
uint8_t* irGetBits(uint8_t* bytes);
void irPulse(uint8_t pin, long microsecs);
void irSend(uint8_t pin, struct IRSettingCfg *settings);

#endif // IR_SEND
