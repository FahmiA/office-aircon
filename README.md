# office-aircon
Aircon control for the office. Based on NodeMCU and ESP8266.

## Build

Development

    platformio lib update # Download dependencies
    ./watch.sh

Deploy

    platformio run --target upload

Deploy Settings
    pip install esptool

Watch

    platformio device monitor

## Settings

http://esp8266.github.io/Arduino/versions/2.1.0-rc1/doc/filesystem.html

Dump settings from flash eeprom partition
    python -m esptool --port /dev/ttyUSB0 read_flash 0x7b000 0x80000 out.bin

Write settings to flash eeprom parition
    python -m esptool --port /dev/ttyUSB0 write_flash 0x7b000 out.bind

0x7b000 is the start of the parition
0x80000 is the parition size (512 bytes)
