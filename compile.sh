#!/bin/sh
g++ -g -std=c++17 -Iinclude -Llib src/main.cpp -lassimp-vc143-mt -lglfw3dll -o src/main.exe
