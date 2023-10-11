#!/bin/bash
mkdir -p build
cd build
cmake -S ../ -B .
make && ./Nugie-Ray-Tracing-Engine
cd ..