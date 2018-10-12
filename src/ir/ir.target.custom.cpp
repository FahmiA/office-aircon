#include "ir.target.custom.hpp"

#include "ir.target.hpp"

// https://github.com/ToniA/arduino-heatpumpir/blob/master/FuegoHeatpumpIR.h

#define IR_NUM_BYTES 14
#define IR_SIZOF_BYTE 8

#define HDR_MARK 3600
#define HDR_SPACE 1630
#define ONE_SPACE 1380
#define ZERO_SPACE 420
#define BIT_MARK 420

// Signal Header
#define HEADER_BYTE_1 0x23
#define HEADER_BYTE_2 0xCB
#define HEADER_BYTE_3 0x26

#define MITSUBISHI_CUSTOM_NAME "mitsubishi_custom"

void irPulse(uint8_t pin, long microsecs);

IRCustomMitsubishiTarget* irGetCustomTarget(const char* model) {
    IRCustomMitsubishiTarget* target = NULL;

    if(strcasecmp(model, MITSUBISHI_CUSTOM_NAME) == 0) {
        target = new IRCustomMitsubishiTarget();
    }

    return target;
}

const char* IRCustomMitsubishiTarget::getName() {
    return MITSUBISHI_CUSTOM_NAME;
}

void IRCustomMitsubishiTarget::send(uint8_t pin, IRSettingCfg *settings) {
    Serial.println("IRCustomMitsubishiTarget.send");

    uint8_t *bytes = this->getSignalBytes(settings);
    uint8_t *bits = this->getSignalBits(bytes);

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
}


uint8_t IRCustomMitsubishiTarget::getPowerCode(IRPower power) {
    switch(power) {
        case IRPower::On: return 0x24;
        default: return 0x20;
    }
}

uint8_t IRCustomMitsubishiTarget::getModeCode(IRMode mode) {
    switch(mode) {
        case IRMode::Auto: return 0x8;
        case IRMode::Heat: return 0x1;
        case IRMode::Dry: return 0x2;
        case IRMode::Cool: return 0x3;
        case IRMode::Fan: return 0x7;
        default: return 0x8;
    }
}

uint8_t IRCustomMitsubishiTarget::getFanSpeedCode(IRFanSpeed fanSpeed) {
    switch(fanSpeed) {
        case IRFanSpeed::Auto: return 0;
        case IRFanSpeed::Low: return 0x02;
        case IRFanSpeed::Medium: return 0x03;
        case IRFanSpeed::High: return 0x05;
        default: return 0;
    }
}

uint8_t IRCustomMitsubishiTarget::getFanVertCode(IRFanVert fanVert) {
    switch(fanVert) {
        case IRFanVert::Auto: return 0;
        case IRFanVert::Up: return 0x08;
        case IRFanVert::MiddleUp: return 0x10;
        case IRFanVert::Middle: return 0x18;
        case IRFanVert::MiddleDown: return 0x20;
        case IRFanVert::Down: return 0x28;
        case IRFanVert::Swing: return 0x38;
        default: return 0;
    }
}

uint8_t IRCustomMitsubishiTarget::getFanHorzCode(IRFanHorz fanHorz) {
    switch(fanHorz) {
        case IRFanHorz::Auto: return 0;
        case IRFanHorz::Left: return 0x04;
        case IRFanHorz::MiddleLeft: return 0x08;
        case IRFanHorz::Middle: return 0x0C;
        case IRFanHorz::MiddleRight: return 0x10;
        case IRFanHorz::Right: return 0x14;
        case IRFanHorz::LeftRight: return 0x20;
        case IRFanHorz::Swing: return 0x30;
        default: return 0;
    }
}

/*
 * Produce sequence checksum.
 */
uint8_t IRCustomMitsubishiTarget::getChecksum(uint8_t* bytes) {
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
uint8_t* IRCustomMitsubishiTarget::getSignalBytes(struct IRSettingCfg *settings) {
    uint8_t *bytes = (uint8_t*) calloc(IR_NUM_BYTES, sizeof(uint8_t));

    bytes[0] = HEADER_BYTE_1;
    bytes[1] = HEADER_BYTE_2;
    bytes[2] = HEADER_BYTE_3;
    bytes[3] = 0x1; // ?

    bytes[5] = this->getPowerCode(settings->power);
    bytes[6] = this->getModeCode(settings->mode);
    bytes[7] = 31 - settings->temp;
    bytes[8] = this->getFanSpeedCode(settings->fanSpeed) | this->getFanVertCode(settings->fanDirVert);

    bytes[12] = this->getFanHorzCode(settings->fanDirHorz);
    bytes[13] = this->getChecksum(bytes);

    return bytes;
}

/*
 * Produce bit sequence from byte sequence.
 */
uint8_t* IRCustomMitsubishiTarget::getSignalBits(uint8_t* bytes) {
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

/*
 * This procedure sends a 38KHz pulse to the IRledPin 
 * for a certain # of microseconds. We'll use this whenever we need to send codes
 */
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

