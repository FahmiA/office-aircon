#include "ir.target.generic.hpp"

#include "ir.target.hpp"

#include <HeatpumpIR.h>
#include <PanasonicCKPHeatpumpIR.h>
#include <PanasonicHeatpumpIR.h>
#include <MitsubishiHeatpumpIR.h>
#include <MitsubishiHeavyHeatpumpIR.h>
#include <MitsubishiSEZKDXXHeatpumpIR.h>

IRGenenricTarget* irGetGenericTarget(const String& model) {
    IRGenenricTarget* target = NULL;

    if(model == "mitsubishi_fd") {
        target = new IRGenenricTarget(new MitsubishiFDHeatpumpIR(), "mitsubishi_fd");
    } else if(model == "mitsubishi_fe") {
        target = new IRGenenricTarget(new MitsubishiFEHeatpumpIR(), "mitsubishi_fe");
    } else if(model == "mitsubishi_msy") {
        target = new IRGenenricTarget(new MitsubishiMSYHeatpumpIR(), "mitsubishi_msy");
    } else if(model == "mitsubishi_fa") {
        target = new IRGenenricTarget(new MitsubishiFAHeatpumpIR(), "mitsubishi_fa");
    } else if(model == "mitsubishi_heavy_zj") {
        target = new IRGenenricTarget(new MitsubishiHeavyZJHeatpumpIR(), "mitsubishi_heavy_zj");
    } else if(model == "mitsubishi_heavy_zm") {
        target = new IRGenenricTarget(new MitsubishiHeavyZMHeatpumpIR(), "mitsubishi_heavy_zm");
    } else if(model == "mitsubishi_heavy_zmp") {
        target = new IRGenenricTarget(new MitsubishiHeavyZMPHeatpumpIR(), "mitsubishi_heavy_zmp");
    } else if(model == "mitsubishi_sez") {
        target = new IRGenenricTarget(new MitsubishiSEZKDXXHeatpumpIR(), "mitsubishi_sez");
    } else if(model == "panasonic_ckp") {
        target = new IRGenenricTarget(new PanasonicCKPHeatpumpIR(), "panasonic_ckp");
    } else if(model == "panasonic_dke") {
        target = new IRGenenricTarget(new PanasonicDKEHeatpumpIR(), "panasonic_dke");
    } else if(model == "panasonic_jke") {
        target = new IRGenenricTarget(new PanasonicJKEHeatpumpIR(), "panasonic_jke");
    } else if(model == "panasonic_nke") {
        target = new IRGenenricTarget(new PanasonicNKEHeatpumpIR(), "panasonic_nke");
    } else if(model == "panasonic_lke") {
        target = new IRGenenricTarget(new PanasonicLKEHeatpumpIR(), "panasonic_lke");
    }

    return target;
};

IRGenenricTarget::IRGenenricTarget(HeatpumpIR *ir, const String model) {
    this->ir = ir;
    this->model = model;
}

IRGenenricTarget::~IRGenenricTarget() {
    delete this->ir;
}

String IRGenenricTarget::getName() {
    return this->model;
}

void IRGenenricTarget::send(uint8_t pin, IRSettingCfg *settings) {
    Serial.println("IRGenenricTarget.send");

    //IRSenderPWM irSender(pin);     // Arduino PWM
    //IRSenderBlaster irSender(pin); // IR Blaster (generates the 38 kHz carrier)
    IRSenderBitBang irSender(pin); // 'bit banging' on ESP8266 etc (also 38 kHz)

    this->ir->send(
        irSender,
        this->getPowerCode(settings->power),
        this->getModeCode(settings->mode),
        this->getFanSpeedCode(settings->fanSpeed),
        settings->temp,
        this->getFanVertCode(settings->fanDirVert),
        this->getFanHorzCode(settings->fanDirHorz)
    );
}

uint8_t IRGenenricTarget::getPowerCode(IRPower power) {
    switch(power) {
        case IRPower::On: return POWER_ON;
        default: return POWER_OFF;
    }
}

uint8_t IRGenenricTarget::getModeCode(IRMode mode) {
    switch(mode) {
        case IRMode::Auto: return MODE_AUTO;
        case IRMode::Heat: return MODE_HEAT;
        case IRMode::Dry: return MODE_DRY;
        case IRMode::Cool: return MODE_COOL;
        case IRMode::Fan: return MODE_FAN;
        default: return MODE_AUTO;
    }
}

uint8_t IRGenenricTarget::getFanSpeedCode(IRFanSpeed fanSpeed) {
    switch(fanSpeed) {
        case IRFanSpeed::Auto: return FAN_AUTO;
        case IRFanSpeed::Low: return FAN_1;
        case IRFanSpeed::Medium: return FAN_2;
        case IRFanSpeed::High: return FAN_3;
        default: return FAN_AUTO;
    }
}

uint8_t IRGenenricTarget::getFanVertCode(IRFanVert fanVert) {
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

uint8_t IRGenenricTarget::getFanHorzCode(IRFanHorz fanHorz) {
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

