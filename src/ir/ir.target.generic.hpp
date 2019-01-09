#ifndef IR_TARGET_GENERIC
#define IR_TARGET_GENERIC

#include <HeatpumpIR.h>

#include "ir.target.hpp"

class IRGenenricTarget : public IRTarget {
    public:
        IRGenenricTarget(HeatpumpIR* ir, const char* model);
        ~IRGenenricTarget();

        const char* getName();
        void send(uint8_t pin, IRSettingCfg *settings);
    private:
        HeatpumpIR *ir;
        const char *model;

        uint8_t getPowerCode(IRPower power);
        uint8_t getChecksum(uint8_t* bytes);
        uint8_t getModeCode(IRMode mode);
        uint8_t getFanSpeedCode(IRFanSpeed fanSpeed);
        uint8_t getFanVertCode(IRFanVert fanVert);
        uint8_t getFanHorzCode(IRFanHorz fanHorz);
};

IRGenenricTarget* irGetGenericTarget(const String&);

#endif // IR_TARGET_GENERIC
