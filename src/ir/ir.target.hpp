#ifndef IR_TARGET
#define IR_TARGET

#include <Arduino.h>

class IRTarget;

enum class IRPower    {Off, On};
enum class IRMode     {Unknown, Auto, Heat, Cool, Dry, Fan};
enum class IRFanSpeed {Unknown, Auto, Low, Medium, High};
enum class IRFanVert  {Unknown, Auto, Up, MiddleUp, Middle, MiddleDown, Down, Swing};
enum class IRFanHorz  {Unknown, Auto, Left, MiddleLeft, Middle, MiddleRight, Right, LeftRight, Swing};

struct IRSettingCfg {
    IRTarget* model;
    IRPower power;
    IRMode mode;
    uint8_t temp;
    IRFanSpeed fanSpeed;
    IRFanVert fanDirVert;
    IRFanHorz fanDirHorz;
};

class IRTarget {
    public:
        virtual ~IRTarget();
        virtual const char* getName() = 0;
        virtual void send(uint8_t pin, IRSettingCfg *settings) = 0;
};

#endif // IR_TARGET
