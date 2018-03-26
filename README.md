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

Update `private.sh` with settings and set `FIRST_TIME_SETUP` to true.

## Circuit

![Board Diagram](./board/Board_export.png)