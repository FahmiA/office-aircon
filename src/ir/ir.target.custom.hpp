#ifndef IR_TARGET_CUSTOM
#define IR_TARGET_CUSTOM

#include "ir.target.hpp"

class IRCustomMitsubishiTarget : public IRTarget {
    public:
        const char* getName();
        void send(uint8_t pin, IRSettingCfg *settings);
    private:
        uint8_t getPowerCode(IRPower power);
        uint8_t getChecksum(uint8_t* bytes);
        uint8_t getModeCode(IRMode mode);
        uint8_t getFanSpeedCode(IRFanSpeed fanSpeed);
        uint8_t getFanVertCode(IRFanVert fanVert);
        uint8_t getFanHorzCode(IRFanHorz fanHorz);
        uint8_t* getSignalBytes(struct IRSettingCfg *settings);
        uint8_t* getSignalBits(uint8_t* bytes);
};

IRCustomMitsubishiTarget* irGetCustomTarget(const char*);

#endif // IR_TARGET_CUSTOM
