#!/bin/bash

platformio run --target clean
find src/ | entr -rc platformio run -e nodemcuv2
