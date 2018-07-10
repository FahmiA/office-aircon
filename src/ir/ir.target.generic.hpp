#ifndef IR_TARGET_GENERIC
#define IR_TARGET_GENERIC

#include <HeatpumpIR.h>

#include "ir.target.hpp"

class IRGenenricTarget : public IRTarget {
    private:
        HeatpumpIR *ir;
    public:
        IRGenenricTarget(HeatpumpIR*);
        const char* getName();
        void send(uint8_t pin, IRSettingCfg *settings);
};

uint8_t irTargetGetId(const char* key);
IRTarget* irTargetGetInstance(uint8_t id);

#endif // IR_TARGET_GENERIC
