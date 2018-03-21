#ifndef CONFIG
#define CONFIG

#define CONFIG_VERSION 1

struct Config {
    const char wifiSSID[20];
    const char wifiPassword[20];
    const char mqttServer[20];
    const int mqttPort;
    const char mqttUsername[20];
    const char mqttPassword[20];
    const char mqttClientID[20];
};

Config* config_load();
void config_set(Config* config);
void config_dump();
void config_print(Config *config);

#endif // CONFIG
