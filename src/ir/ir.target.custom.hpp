#ifndef IR_TARGET_CUSTOM
#define IR_TARGET_CUSTOM

#include "ir.target.hpp"

class IRCustomMitsubishiTarget : public IRTarget {
    public:
        const char* getName();
        void send(uint8_t pin, IRSettingCfg *settings);
};

IRCustomMitsubishiTarget* irGetCustomTarget(const char*);

#endif // IR_TARGET_CUSTOM
