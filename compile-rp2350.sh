#!/bin/bash
rm -rf build-2350 
mkdir build-2350
cd build-2350
cmake .. -DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350 
make

