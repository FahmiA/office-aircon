#include <Arduino.h>
#include <Homie.h>
#include <DHT.h>

#include "ir/ir.target.hpp"
#include "ir/ir.target.custom.hpp"
#include "ir/ir.target.generic.hpp"

// LoLin NodeMCU pins (https://github.com/esp8266/Arduino/issues/584)
#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1

// https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59
#define IRLED_PIN D5

#define DHT_TYPE DHT11
#define DHT_PIN D4

#define IR_EVENT_DELAY_MS 2000 // 2 seconds
#define TEMP_EVENT_DELAY_MS 300000 // 5 minutes

void sendIRSequence();
void takeTempReading();

IRSettingCfg settings { new IRCustomMitsubishiTarget(), IRPower::Off, IRMode::Auto, (uint8_t) 21, IRFanSpeed::Auto, IRFanVert::Auto, IRFanHorz::Auto };
unsigned long lastIREventMS = 0;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastTempEventMS = 0;

HomieNode temperatureNode("temperature", "temperature");
HomieNode airconNode("aircon", "hvac");

void setupHandler() {
    dht.begin();

    lastTempEventMS = millis();
}

void loopHandler() {
    unsigned long ms = millis();

    if(lastIREventMS > 0 && ms - lastIREventMS > IR_EVENT_DELAY_MS) {
        lastIREventMS = 0;
        sendIRSequence();
    }

    if(ms - lastTempEventMS > TEMP_EVENT_DELAY_MS) {
        lastTempEventMS = ms;
        takeTempReading();
    }
}

bool setAirconModel(const HomieRange& range, const String& value) {
    IRTarget* target = irGetCustomTarget(value);

    if(target == NULL) {
        target = irGetGenericTarget(value);
    }

    if(target == NULL) {
        return false;
    }

    delete settings.model;
    settings.model = target;
    lastIREventMS = millis();

    Homie.getLogger() << "Set Model: " << target->getName() << endl;
    airconNode.setProperty("model").send(target->getName());

    return true;
}

bool setAirconPower(const HomieRange& range, const String& value) {
    IRPower power;
    String text;

    if(value == "on" || value == "true" || value == "t" || value == "yes" || value == "y" || value == "1") {
        power = IRPower::On;
        text = "on";
    } else if(value == "off" || value == "false" || value == "f" || value == "no" || value == "n" || value == "0") {
        power = IRPower::Off;
        text = "off";
    } else {
        return false;
    }

    settings.power = power;
    lastIREventMS = millis();

    Homie.getLogger() << "Set Power: " << text << endl;
    airconNode.setProperty("power").send(text);

    return true;
}

bool setAirconMode(const HomieRange& range, const String& value) {
    IRMode mode = IRMode::Unknown;
    String text;

    if(value == "a" || value == "auto") {
        mode = IRMode::Auto;
        text = "auto";
    } else if(value == "h" || value == "heat") {
        mode = IRMode::Heat;
        text = "heat";
    } else if(value == "c" || value == "cool") {
        mode = IRMode::Cool;
        text = "cool";
    } else if(value == "d" || value == "dry") {
        mode = IRMode::Dry;
        text = "dry";
    } else if(value == "f" || value == "fan") {
        mode = IRMode::Fan;
        text = "fan";
    }
    
    if(mode == IRMode::Unknown) {
        return false;
    }

    Homie.getLogger() << "Set Mode: " << text << endl;
    airconNode.setProperty("mode").send(text);

    settings.mode = mode;
    lastIREventMS = millis();

    return true;
}

bool setAirconTemperature(const HomieRange& range, const String& value) {
    int temperature = value.toInt();

    if(temperature < 18 || temperature > 32) {
        return false;
    }

    settings.temp = temperature;
    lastIREventMS = millis();

    String text = String(temperature);
    Homie.getLogger() << "Set Temperature: " << text << endl;
    airconNode.setProperty("temperature").send(text);

    return true;
}

bool setAirconFanSpeed(const HomieRange& range, const String& value) {
    IRFanSpeed speed = IRFanSpeed::Unknown;
    String text;

    if(value == "a" || value == "auto") {
        speed = IRFanSpeed::Auto;
        text = "auto";
    } else if(value == "l" || value == "low") {
        speed = IRFanSpeed::Low;
        text = "low";
    } else if(value == "m" || value == "medium") {
        speed = IRFanSpeed::Medium;
        text = "medium";
    } else if(value == "h" || value == "high") {
        speed = IRFanSpeed::High;
        text = "high";
    }
    
    if(speed == IRFanSpeed::Unknown) {
        return false;
    }

    Homie.getLogger() << "Set Fan Speed: " << text << endl;
    airconNode.setProperty("fan-speed").send(text);

    settings.fanSpeed = speed;
    lastIREventMS = millis();

    return true;
}

bool setAirconFanDirVert(const HomieRange& range, const String& value) {
    IRFanVert vert = IRFanVert::Unknown;
    String text;

    if(value == "a" || value == "auto") {
        vert = IRFanVert::Auto;
        text = "auto";
    } else if(value == "u" || value == "up") {
        vert = IRFanVert::Up;
        text = "up";
    } else if(value == "m" || value == "middle") {
        vert = IRFanVert::Middle;
        text = "middle";
    } else if(value == "mu" || value == "middle-up") {
        vert = IRFanVert::MiddleUp;
        text = "middle-up";
    } else if(value == "md" || value == "middle-down") {
        vert = IRFanVert::MiddleDown;
        text = "middle-down";
    } else if(value == "d" || value == "down") {
        vert = IRFanVert::Down;
        text = "down";
    } else if(value == "s" || value == "swing") {
        vert = IRFanVert::Swing;
        text = "swing";
    }
    
    if(vert == IRFanVert::Unknown) {
        return false;
    }

    Homie.getLogger() << "Set Fan Vert: " << text << endl;
    airconNode.setProperty("fan-vert").send(text);

    settings.fanDirVert = vert;
    lastIREventMS = millis();

    return true;
}

bool setAirconFanDirHorz(const HomieRange& range, const String& value) {
    IRFanHorz vert = IRFanHorz::Unknown;
    String text;

    if(value == "a" || value == "auto") {
        vert = IRFanHorz::Auto;
        text = "auto";
    } else if(value == "l" || value == "left") {
        vert = IRFanHorz::Left;
        text = "left";
    } else if(value == "lr" || value == "left-right") {
        vert = IRFanHorz::LeftRight;
        text = "left-right";
    } else if(value == "m" || value == "middle") {
        vert = IRFanHorz::Middle;
        text = "middle";
    } else if(value == "ml" || value == "middle-left") {
        vert = IRFanHorz::MiddleLeft;
        text = "middle-left";
    } else if(value == "mr" || value == "middle-right") {
        vert = IRFanHorz::MiddleRight;
        text = "middle-right";
    } else if(value == "r" || value == "right") {
        vert = IRFanHorz::Right;
        text = "right";
    } else if(value == "s" || value == "swing") {
        vert = IRFanHorz::Swing;
        text = "swing";
    }
    
    if(vert == IRFanHorz::Unknown) {
        return false;
    }

    Homie.getLogger() << "Set Fan Horz: " << text << endl;
    airconNode.setProperty("fan-horz").send(text);

    settings.fanDirHorz = vert;
    lastIREventMS = millis();

    return true;
}

void setup() {
    Serial.begin(9600);
    Serial << endl << endl;

    pinMode(IRLED_PIN, OUTPUT);
    randomSeed(micros());

    Homie_setFirmware("office-aircon", "0.9.1");
    Homie.setHomieBootMode(HomieBootMode::NORMAL); // Never start an AP to configure device (insecure)
    Homie.setSetupFunction(setupHandler);
    Homie.setLoopFunction(loopHandler);

    temperatureNode.advertise("temperature");
    temperatureNode.advertise("temperature-feel");
    temperatureNode.advertise("humidity");

    airconNode.advertise("model").settable(setAirconModel);
    airconNode.advertise("power").settable(setAirconPower);
    airconNode.advertise("mode").settable(setAirconMode);
    airconNode.advertise("temperature").settable(setAirconTemperature);
    airconNode.advertise("fan-speed").settable(setAirconFanSpeed);
    airconNode.advertise("fan-vert").settable(setAirconFanDirVert);
    airconNode.advertise("fan-horz").settable(setAirconFanDirHorz);

    Homie.setup();
}

void loop () {
    Homie.loop();
}

void sendIRSequence() {
    Homie.getLogger().print("Sending IR sequence... ");
    settings.model->send(IRLED_PIN, &settings);
    Homie.getLogger().println("Done");
}

void takeTempReading() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Homie.getLogger().println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Homie.getLogger() << "Humidity: " << h << "%\t";
    Homie.getLogger() << "Temperature: " << t << "*C\t";
    Homie.getLogger() << "Heat Index: " << hic << "*C" << endl;

    temperatureNode.setProperty("temperature").send(String(t));
    temperatureNode.setProperty("temperature-sense").send(String(hic));
    temperatureNode.setProperty("humidity").send(String(h));
}

