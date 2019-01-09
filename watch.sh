#!/bin/bash

platformio run --target clean
find src -type f | entr -rc platformio run
