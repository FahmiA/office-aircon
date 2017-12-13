#!/bin/bash

platformio run --target clean
find src -name '*.hpp' -o -name '*.cpp' | entr -rc platformio run -e nodemcuv2
