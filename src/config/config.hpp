#ifndef CONFIG
#define CONFIG

#define CONFIG_VERSION 1

struct Config {
    int version;
    char wifiSSID[20];
    char wifiPassword[20];
    char mqttServer[20];
    int mqttPort;
    char mqttUsername[20];
    char mqttPassword[20];
    char mqttClientID[20];
};

Config* config_load();
Config* config_loadFromEnvironment();
void config_set(Config* config);
void config_dump();
void config_print(Config *config);
char* config_getName(Config *config);

#endif // CONFIG
