#ifndef IR_PARSE
#define IR_PARSE

#include "ir.send.hpp"

bool irParseBool(byte* payload);
uint8_t irParseInt(byte* payload);
char* irParseString(byte* payload);

IRSetting* irParsePower(bool power);
IRSetting* irParseMode(const char* value);
IRSetting* irParseTemp(uint8_t value);
IRSetting* irParseFanSpeed(const char* value);
IRSetting* irParseFanDirVert(const char* value);
IRSetting* irParseFanDirHorz(const char* value);

#endif // IR_PARSE
