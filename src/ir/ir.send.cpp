#include "ir.send.hpp"

#define HDR_MARK 3600
#define HDR_SPACE 1630
#define ONE_SPACE 1380
#define ZERO_SPACE 420
#define BIT_MARK 420

// https://github.com/ToniA/arduino-heatpumpir/blob/master/FuegoHeatpumpIR.h

// Power
IRSetting PowerOff = {"Power:Off", 0x20};
IRSetting PowerOn = {"Power:On", 0x24};

// Mode
IRSetting ModeAuto = {"Mode:Auto", 0x8};
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

// Signal Header
uint8_t HEADER_BYTE_1 = 0x23;
uint8_t HEADER_BYTE_2 = 0xCB;
uint8_t HEADER_BYTE_3 = 0x26;

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
 * Produce byte sequence.
 *
 * https://github.com/ToniA/Raw-IR-decoder-for-Arduino/blob/master/Fuego.cpp
 */
uint8_t* irGetBytes(struct IRSettingCfg *settings) {
    uint8_t *bytes = (uint8_t*) calloc(IR_NUM_BYTES, sizeof(uint8_t));

    bytes[0] = HEADER_BYTE_1;
    bytes[1] = HEADER_BYTE_2;
    bytes[2] = HEADER_BYTE_3;
    bytes[3] = 0x1; // ?

    bytes[5] = settings->power.value;
    bytes[6] = settings->mode.value;
    bytes[7] = 31 - settings->temp.value;
    bytes[8] = settings->fanSpeed.value | settings->fanDirVert.value;

    bytes[12] = settings->fanDirHorz.value;
    bytes[13] = irGetChecksum(bytes);

    return bytes;
}

/*
 * Produce bit sequence from byte sequence.
 */
uint8_t* irGetBits(uint8_t* bytes) {
    uint8_t *bits = (uint8_t*) calloc(IR_NUM_BYTES * IR_SIZOF_BYTE, sizeof(uint8_t));
    // uint8_t mask = 0x80
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

// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void irPulse(uint8_t pin, long microsecs) {
    // we'll count down from the number of microseconds we are told to wait

    cli();  // this turns off any background interrupts

    while (microsecs > 0) {
        // 38 kHz is about 13 microseconds high and 13 microseconds low
        digitalWrite(pin, HIGH);  // this takes about 3 microseconds to happen
        delayMicroseconds(8);          // hang out for 10 microseconds, you can also change this to 9 if its not working
        digitalWrite(pin, LOW);   // this also takes about 3 microseconds
        delayMicroseconds(8);          // hang out for 10 microseconds, you can also change this to 9 if its not working

        // so 26 microseconds altogether
        microsecs -= 26;
    }

    sei();  // this turns them back on
}

/*
 * Blast IR settings.
 */
void irSend(uint8_t pin, struct IRSettingCfg *settings) {
    // TODO: Blast that IR!
    uint8_t *bytes = irGetBytes(settings);
    uint8_t *bits = irGetBits(bytes);

    // Initiate signal
    irPulse(pin, HDR_MARK);
    delayMicroseconds(HDR_SPACE);

    // Body of signal
    irPulse(pin, ZERO_SPACE);
    for (uint8_t i = 0; i < (IR_NUM_BYTES * IR_SIZOF_BYTE); i++) {
        if (bits[i]) {
            delayMicroseconds(ONE_SPACE);
        } else {
            delayMicroseconds(ZERO_SPACE);
        }
        irPulse(pin, BIT_MARK);
    }

    free(bits);
}

