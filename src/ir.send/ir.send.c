#include <stdio.h>
#include <stdlib.h>
#include "ir.send.h"

// Power
IRSetting PowerOff = {"Power:Off", 0x20};
IRSetting PowerOn = {"Power:On", 0x24};

// Mode
IRSetting ModeAuto = {"Mode:Auto", 0};
IRSetting ModeHeat = {"Mode:Heat", 0x1};
IRSetting ModeDry = {"Mode:Dry", 0x2};
IRSetting ModeCool = {"Mode:Cool", 0x3};
IRSetting ModeFan = {"Mode:Fan", 0x7};

// Fan Speed
IRSetting FanSpeedAuto = { "FanSpeed:Auto", 0 };
IRSetting FanSpeedLow = {"FanSpeed:Low", 0x02 };
IRSetting FanSpeedMed = {"FanSpeed:Med", 0x03 };
IRSetting FanSpeedHigh = {"FanSpeed:High", 0x05 };

// Fan Vertical Direction (Vane)
IRSetting FanVertAuto = {"FanVert:Auto", 0 };
IRSetting FanVertUp = {"FanVert:Up", 0x08};
IRSetting FanVertMiddleUp = {"FanVert:MiddleUp", 0x10};
IRSetting FanVertMiddle = {"FanVert:Middle", 0x18};
IRSetting FanVertMiddleDown = {"FanVert:MiddleDown", 0x20};
IRSetting FanVertDown = {"FanVert:Down", 0x28};
IRSetting FanVertSwing = {"FanVert:Swing", 0x38};

// Fan Horizontal Direction (Wide Vane)
IRSetting FanHorzAuto = {"FanHorz:Auto", 0 };
IRSetting FanHorzLeft = {"FanHorz:Left", 0x04 };
IRSetting FanHorzMiddleLeft = {"FanHorz:MiddleLeft", 0x08 };
IRSetting FanHorzMiddle = {"FanHorz:Middle", 0x0C };
IRSetting FanHorzMiddleRight = {"FanHorz:MiddleRight", 0x10 };
IRSetting FanHorzRight = {"FanHorz:Right", 0x14 };
IRSetting FanHorzLeftRight = {"FanHorz:LeftRight", 0x20 };
IRSetting FanHorzSwing = {"FanHorz:LeftRight", 0x30 };
/*IRSetting FanHorzMiddleRight = {"FanHorz:MiddleRight", 0x14 };*/
/*IRSetting FanHorzRight = {"FanHorz:Right", 0x20 };*/
/*IRSetting FanHorzLeftRight = {"FanHorz:LeftRight", 0x30 };*/

// Signal Header
uint8_t HEADER_BYTE_1 = 0x23;
uint8_t HEADER_BYTE_2 = 0xCB;
uint8_t HEADER_BYTE_3 = 0x26;

/*
 * Produce byte sequence.
 *
 * https://github.com/ToniA/Raw-IR-decoder-for-Arduino/blob/master/Fuego.cpp
 */
uint8_t* irGetBytes(struct IRSettingCfg *settings) {
    uint8_t *bytes = calloc(IR_NUM_BYTES, sizeof(uint8_t));

    bytes[0] = HEADER_BYTE_1;
    bytes[1] = HEADER_BYTE_2;
    bytes[2] = HEADER_BYTE_3;
    bytes[3] = 0x1; // ?

    bytes[5] = settings->power.value;
    bytes[6] = settings->mode.value;
    bytes[7] = 31 - settings->temp.value;
    bytes[8] = settings->fanSpeed.value | settings->fanDirVert.value;
    /*bytes[8] = settings->fanDirVert.value << 3;*/

    bytes[12] = settings->fanDirHorz.value;
    bytes[13] = irGetChecksum(bytes);

    return bytes;
}

/*
 * Produce sequence checksum.
 */
uint8_t irGetChecksum(uint8_t* bytes) {
    int checksum = 0;

    for(int i = 0; i < IR_NUM_BYTES; i++) {
        checksum += bytes[i];
    }

    return checksum;
}

/*
 * Produce bit sequence from byte sequence.
 */
uint8_t* irGetBits(uint8_t* bytes) {
    uint8_t *bits = calloc(IR_NUM_BYTES * IR_SIZOF_BYTE, sizeof(uint8_t));
    /*uint8_t mask = 0x80;*/
    uint8_t mask = 0x01; // Flip for some reason
    int i = 0;

    for(int byteIndex = 0; byteIndex < IR_NUM_BYTES; byteIndex++) {
        for (int bitIndex = 0; bitIndex < IR_SIZOF_BYTE; bitIndex++) {
            bits[i] = ((bytes[byteIndex] >> bitIndex) & mask) ? 1 : 0;
            i += 1;
        }
    }

    return bits;
}

/*
 * Blast IR settings.
 */
void irSend(struct IRSettingCfg *settings) {
    // TODO: Blast that IR!
    uint8_t *bytes = irGetBytes(settings);
    uint8_t *bits = irGetBits(bytes);

    printf("Bits: ");
    for(int i = 0; i < IR_NUM_BYTES; i++) {
        printf("%d", bits[i]);
    }
    printf("\n");
    
}
