#!/usr/bin/env bash
set -e
sudo apt update
sudo apt install -y build-essential cmake git curl
[ -d third_party/rpi-rgb-led-matrix/lib ] || git clone --depth 1 https://github.com/hzeller/rpi-rgb-led-matrix.git third_party/rpi-rgb-led-matrix
mkdir -p third_party/stb && curl -fsSL -o third_party/stb/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h && curl -fsSL -o third_party/stb/stb_image_resize2.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_resize2.h
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DON_RPI=ON
cmake --build build -j2