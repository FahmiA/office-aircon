#ifndef IR_TARGET_GENERIC
#define IR_TARGET_GENERIC

#include <HeatpumpIR.h>

#include "ir.target.hpp"

class IRGenenricTarget : public IRTarget {
    private:
        HeatpumpIR *ir;
        const char *model;
    public:
        IRGenenricTarget(HeatpumpIR* ir, const char* model);
        ~IRGenenricTarget();
        const char* getName();
        void send(uint8_t pin, IRSettingCfg *settings);
};

IRGenenricTarget* irGetGenericTarget(const char*);

#endif // IR_TARGET_GENERIC
