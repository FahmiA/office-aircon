#ifndef IR_SEND
#define IR_SEND

#include <stdint.h>

#define IR_NUM_BYTES 14
#define IR_SIZOF_BYTE 8

typedef struct {
    char* name;
    uint8_t value;
} IRSetting;

IRSetting PowerOff;
IRSetting PowerOn;

IRSetting ModeAuto;
IRSetting ModeHeat;
IRSetting ModeCool;
IRSetting ModeDry;
IRSetting ModeFan;

IRSetting FanSpeedAuto;
IRSetting FanSpeedLow;
IRSetting FanSpeedMed;
IRSetting FanSpeedHigh;

IRSetting FanVertAuto;
IRSetting FanVertUp;
IRSetting FanVertMiddleUp;
IRSetting FanVertMiddle;
IRSetting FanVertMiddleDown;
IRSetting FanVertDown;
IRSetting FanVertSwing;

struct IRSettingCfg {
    IRSetting power;
    IRSetting mode;
    IRSetting temp;
    IRSetting fanSpeed;
    IRSetting fanDirVert;
};

uint8_t* irGetBytes(struct IRSettingCfg *settings);
uint8_t irGetChecksum(uint8_t* bytes);
uint8_t* irGetBits(uint8_t* bytes);
void irSend(struct IRSettingCfg *settings);

#endif // IR_SEND
