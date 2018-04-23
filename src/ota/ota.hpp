#ifndef OTA
#define OTA

#include <ArduinoOTA.h>

#include "../config/config.hpp"

void ota_setup(Config* config);
void ota_loop();

#endif // OTA
