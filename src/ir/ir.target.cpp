#include "ir.target.hpp"

#include <HeatpumpIR.h>
#include <PanasonicCKPHeatpumpIR.h>
#include <PanasonicHeatpumpIR.h>
#include <MitsubishiHeatpumpIR.h>
#include <MitsubishiHeavyHeatpumpIR.h>
#include <MitsubishiSEZKDXXHeatpumpIR.h>

IRGenenricTarget::IRGenenricTarget(HeatpumpIR *ir) {
    this->ir = ir;
}

const char* IRGenenricTarget::getName() {
    return this->ir->model();
}

void IRGenenricTarget::send(IRSettingCfg *settings) {
    Serial.println("IRGenenricTarget.send");
}

const char* IRCustomMitsubishiTarget::getName() {
    return "mitsubishi-custom";
}

void IRCustomMitsubishiTarget::send(IRSettingCfg *settings) {
    Serial.println("IRCustomMitsubishiTarget.send");
}

HeatpumpIR *registeredHeatpumpIRs[] = {
    // Mitsubishi
    new MitsubishiFDHeatpumpIR(),
    new MitsubishiFEHeatpumpIR(),
    new MitsubishiMSYHeatpumpIR(),
    new MitsubishiFAHeatpumpIR(),
    new MitsubishiHeavyZJHeatpumpIR(),
    new MitsubishiHeavyZMPHeatpumpIR(),
    new MitsubishiHeavyZMPHeatpumpIR(),
    new MitsubishiSEZKDXXHeatpumpIR(),
    // Panasonic
    new PanasonicCKPHeatpumpIR(),
    new PanasonicDKEHeatpumpIR(),
    new PanasonicJKEHeatpumpIR(),
    new PanasonicNKEHeatpumpIR(),
    new PanasonicLKEHeatpumpIR()
};

uint8_t irTargetGetId(const char* key) {
    int count = sizeof(registeredHeatpumpIRs) / sizeof(*registeredHeatpumpIRs);

    int id = 0;
    for(int i = 0; i < count; i++) {
        if(strcasecmp(key, registeredHeatpumpIRs[i]->model()) == 0) {
            id = i + 1;
            break;
        }
    }

    return id;
}

IRTarget* irTargetGetInstance(int id) {
    int count = sizeof(registeredHeatpumpIRs) / sizeof(*registeredHeatpumpIRs);
    if(id <= 0 || id >= count) {
        return new IRCustomMitsubishiTarget();
    }

    HeatpumpIR *heatpumpIR = registeredHeatpumpIRs[id];
    return new IRGenenricTarget(heatpumpIR);
}
