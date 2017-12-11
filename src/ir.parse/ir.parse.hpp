#ifndef IR_PARSE
#define IR_PARSE

#include "../ir.send/ir.send.hpp"

IRSetting* irParsePower(bool power);
IRSetting* irParseMode(const char* value);
IRSetting* irParseTemp(int value);
IRSetting* irParseFanSpeed(const char* value);
IRSetting* irParseFanDirVert(const char* value);
IRSetting* irParseFanDirHorz(const char* value);

#endif // IR_PARSE