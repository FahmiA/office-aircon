#ifndef IR_PARSE
#define IR_PARSE

#include "ir.target.hpp"

bool irParseBool(byte* payload);
uint8_t irParseInt(byte* payload);
char* irParseString(byte* payload);

IRTarget* irParseModel(const char* value);
IRPower irParsePower(bool power);
IRMode irParseMode(const char* value);
uint8_t irParseTemp(uint8_t value);
IRFanSpeed irParseFanSpeed(const char* value);
IRFanVert irParseFanDirVert(const char* value);
IRFanHorz irParseFanDirHorz(const char* value);

#endif // IR_PARSE
