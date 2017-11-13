#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../src/ir.send/ir.send.h"

void testIRSend() {
    struct IRSettingCfg settings;
    settings.power = PowerOn;
    settings.mode = ModeCool;
    settings.fanSpeed = FanSpeedHigh;
    settings.fanDirVert = FanVertDown;
    settings.fanDirHorz = FanHorzSwing;

    IRSetting temp = {"Temp", 19};
    settings.temp = temp;

    printf("Settings\n");
    printf("\tPower:\t\t%s\n", settings.power.name);
    printf("\tMode:\t\t%s\n", settings.mode.name);
    printf("\tTemp:\t\t%s\t\t%d\n", settings.temp.name, settings.temp.value);
    printf("\tFan Speed:\t%s\n", settings.fanSpeed.name);
    printf("\tFan Vert:\t%s\n", settings.fanDirVert.name);

    uint8_t *bytes = irGetBytes(&settings);
    printf("\n\tBytes:\n\t\t");
    for(int i = 0; i < IR_NUM_BYTES; i++) {
        printf("%.2d ", i);
    }
    printf("\n\t\t");
    for(int i = 0; i < IR_NUM_BYTES; i++) {
        printf("%.2X ", bytes[i]);
    }
    printf("\n");

    uint8_t *bits = irGetBits(bytes);
    printf("\tBits:\n\t\t");
    for(int i = 0; i < (IR_NUM_BYTES * IR_SIZOF_BYTE); i++) {
        if (i % 8 == 0) {
            printf("%d ", i / 8);
        } else {
            printf(" ");
        }
    }
    printf("\n\t\t");
    for(int i = 0; i < (IR_NUM_BYTES * IR_SIZOF_BYTE); i++) {
        if ((i + 1) % 8 == 0) {
            printf("%d ", bits[i]);
        } else {
            printf("%d", bits[i]);
        }
    }
    printf("\n");

    free(bytes);
    free(bits);
}

int main() {
    testIRSend();
    return 0;
}
