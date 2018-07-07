#ifndef IR_TARGET
#define IR_TARGET

#include <HeatpumpIR.h>

#include "ir.send.hpp"

class IRTarget {
    public:
        virtual const char* getName() = 0;
        virtual void send(IRSettingCfg *settings) = 0;
};

class IRGenenricTarget : public IRTarget {
    private:
        HeatpumpIR *ir;
    public:
        IRGenenricTarget(HeatpumpIR*);
        const char* getName();
        void send(IRSettingCfg *settings);
};

class IRCustomMitsubishiTarget : public IRTarget {
    public:
        const char* getName();
        void send(IRSettingCfg *settings);
};

uint8_t irTargetGetId(const char* key);
IRTarget* irTargetGetInstance(int id);


#endif // IR_TARGET
