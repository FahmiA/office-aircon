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

IRGenenricTarget::IRGenenricTarget(HeatpumpIR *ir) {
    this->ir = ir;
}

IRGenenricTarget::~IRGenenricTarget() {
    delete this->ir;
}

const char* IRGenenricTarget::getName() {
    return this->ir->model();
}

void IRGenenricTarget::send(uint8_t pin, IRSettingCfg *settings) {
    //IRSenderPWM irSender(pin);     // Arduino PWM
    IRSenderBlaster irSender(pin); // IR Blaster (generates the 38 kHz carrier)
    //IRSenderBitBang irSender(pin); // IR Blaster (generates the 38 kHz carrier)

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

    if(strcasecmp(model, "mitsubishi_fd")) {
        target = new IRGenenricTarget(new MitsubishiFDHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_fd")) {
        target = new IRGenenricTarget(new MitsubishiFDHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_fe")) {
        target = new IRGenenricTarget(new MitsubishiFEHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_msy")) {
        target = new IRGenenricTarget(new MitsubishiMSYHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_fa")) {
        target = new IRGenenricTarget(new MitsubishiFAHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_heavy_zj")) {
        target = new IRGenenricTarget(new MitsubishiHeavyZJHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_heavy_zm")) {
        target = new IRGenenricTarget(new MitsubishiHeavyZMHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_heavy_zmp")) {
        target = new IRGenenricTarget(new MitsubishiHeavyZMPHeatpumpIR());
    } else if(strcasecmp(model, "mitsubishi_sez")) {
        target = new IRGenenricTarget(new MitsubishiSEZKDXXHeatpumpIR());
    } else if(strcasecmp(model, "panasonic_ckp")) {
        target = new IRGenenricTarget(new PanasonicCKPHeatpumpIR());
    } else if(strcasecmp(model, "panasonic_dke")) {
        target = new IRGenenricTarget(new PanasonicDKEHeatpumpIR());
    } else if(strcasecmp(model, "panasonic_jke")) {
        target = new IRGenenricTarget(new PanasonicJKEHeatpumpIR());
    } else if(strcasecmp(model, "panasonic_nke")) {
        target = new IRGenenricTarget(new PanasonicNKEHeatpumpIR());
    } else if(strcasecmp(model, "panasonic_lke")) {
        target = new IRGenenricTarget(new PanasonicLKEHeatpumpIR());
    }

    return target;
};

