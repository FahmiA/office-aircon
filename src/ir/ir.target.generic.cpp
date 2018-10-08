#include "ir.target.generic.hpp"

#include "ir.target.hpp"

#include <HeatpumpIR.h>
#include <PanasonicCKPHeatpumpIR.h>
#include <PanasonicHeatpumpIR.h>
#include <MitsubishiHeatpumpIR.h>
#include <MitsubishiHeavyHeatpumpIR.h>
#include <MitsubishiSEZKDXXHeatpumpIR.h>


// TODO: Namespace this
uint8_t irGenGetPowerCode(IRPower power) {
    switch(power) {
        case IRPower::On: return POWER_ON;
        default: return POWER_OFF;
    }
}

uint8_t irGenGetModeCode(IRMode mode) {
    switch(mode) {
        case IRMode::Auto: return MODE_AUTO;
        case IRMode::Heat: return MODE_HEAT;
        case IRMode::Dry: return MODE_DRY;
        case IRMode::Cool: return MODE_COOL;
        case IRMode::Fan: return MODE_FAN;
        default: return MODE_AUTO;
    }
}

uint8_t irGenGetFanSpeedCode(IRFanSpeed fanSpeed) {
    switch(fanSpeed) {
        case IRFanSpeed::Auto: return FAN_AUTO;
        case IRFanSpeed::Low: return FAN_1;
        case IRFanSpeed::Medium: return FAN_2;
        case IRFanSpeed::High: return FAN_3;
        default: return FAN_AUTO;
    }
}

uint8_t irGenGetFanVertCode(IRFanVert fanVert) {
    switch(fanVert) {
        case IRFanVert::Auto: return VDIR_AUTO;
        case IRFanVert::Up: return VDIR_UP;
        case IRFanVert::MiddleUp: return VDIR_MUP;
        case IRFanVert::Middle: return VDIR_MIDDLE;
        case IRFanVert::MiddleDown: return VDIR_MDOWN;
        case IRFanVert::Down: return VDIR_DOWN;
        case IRFanVert::Swing: return VDIR_SWING;
        default: return VDIR_AUTO;
    }
}

uint8_t irGenGetFanHorzCode(IRFanHorz fanHorz) {
    switch(fanHorz) {
        case IRFanHorz::Auto: return HDIR_AUTO;
        case IRFanHorz::Left: return HDIR_LEFT;
        case IRFanHorz::MiddleLeft: return HDIR_MLEFT;
        case IRFanHorz::Middle: return HDIR_MIDDLE;
        case IRFanHorz::MiddleRight: return HDIR_MRIGHT;
        case IRFanHorz::Right: return HDIR_RIGHT;
        case IRFanHorz::LeftRight: return HDIR_AUTO; // Not supported by HeatpumpIR
        case IRFanHorz::Swing: return HDIR_SWING;
        default: return HDIR_AUTO;
    }
}

IRGenenricTarget::IRGenenricTarget(HeatpumpIR *ir, const char *model) {
    this->ir = ir;
    this->model = model;
}

IRGenenricTarget::~IRGenenricTarget() {
    delete this->ir;
    delete this->model;
}

const char* IRGenenricTarget::getName() {
    // TODO: Fetch this->ir->model() from PROGMEM
    return this->model;
}

void IRGenenricTarget::send(uint8_t pin, IRSettingCfg *settings) {
    Serial.println("IRGenenricTarget.send");

    //IRSenderPWM irSender(pin);     // Arduino PWM
    //IRSenderBlaster irSender(pin); // IR Blaster (generates the 38 kHz carrier)
    IRSenderBitBang irSender(pin); // 'bit banging' on ESP8266 etc (also 38 kHz)

    this->ir->send(
        irSender,
        irGenGetPowerCode(settings->power),
        irGenGetModeCode(settings->mode),
        irGenGetFanSpeedCode(settings->fanSpeed),
        settings->temp,
        irGenGetFanVertCode(settings->fanDirVert),
        irGenGetFanHorzCode(settings->fanDirHorz)
    );
}

IRGenenricTarget* irGetGenericTarget(const char* model) {
    IRGenenricTarget* target = NULL;

    if(strcasecmp(model, "mitsubishi_fd") == 0) {
        target = new IRGenenricTarget(new MitsubishiFDHeatpumpIR(), "mitsubishi_fd");
    } else if(strcasecmp(model, "mitsubishi_fd") == 0) {
        target = new IRGenenricTarget(new MitsubishiFDHeatpumpIR(), "mitsubishi_fd");
    } else if(strcasecmp(model, "mitsubishi_fe") == 0) {
        target = new IRGenenricTarget(new MitsubishiFEHeatpumpIR(), "mitsubishi_fe");
    } else if(strcasecmp(model, "mitsubishi_msy") == 0) {
        target = new IRGenenricTarget(new MitsubishiMSYHeatpumpIR(), "mitsubishi_msy");
    } else if(strcasecmp(model, "mitsubishi_fa") == 0) {
        target = new IRGenenricTarget(new MitsubishiFAHeatpumpIR(), "mitsubishi_fa");
    } else if(strcasecmp(model, "mitsubishi_heavy_zj") == 0) {
        target = new IRGenenricTarget(new MitsubishiHeavyZJHeatpumpIR(), "mitsubishi_heavy_zj");
    } else if(strcasecmp(model, "mitsubishi_heavy_zm") == 0) {
        target = new IRGenenricTarget(new MitsubishiHeavyZMHeatpumpIR(), "mitsubishi_heavy_zm");
    } else if(strcasecmp(model, "mitsubishi_heavy_zmp") == 0) {
        target = new IRGenenricTarget(new MitsubishiHeavyZMPHeatpumpIR(), "mitsubishi_heavy_zmp");
    } else if(strcasecmp(model, "mitsubishi_sez") == 0) {
        target = new IRGenenricTarget(new MitsubishiSEZKDXXHeatpumpIR(), "mitsubishi_sez");
    } else if(strcasecmp(model, "panasonic_ckp") == 0) {
        target = new IRGenenricTarget(new PanasonicCKPHeatpumpIR(), "panasonic_ckp");
    } else if(strcasecmp(model, "panasonic_dke") == 0) {
        target = new IRGenenricTarget(new PanasonicDKEHeatpumpIR(), "panasonic_dke");
    } else if(strcasecmp(model, "panasonic_jke") == 0) {
        target = new IRGenenricTarget(new PanasonicJKEHeatpumpIR(), "panasonic_jke");
    } else if(strcasecmp(model, "panasonic_nke") == 0) {
        target = new IRGenenricTarget(new PanasonicNKEHeatpumpIR(), "panasonic_nke");
    } else if(strcasecmp(model, "panasonic_lke") == 0) {
        target = new IRGenenricTarget(new PanasonicLKEHeatpumpIR(), "panasonic_lke");
    }

    return target;
};

