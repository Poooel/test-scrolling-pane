#!/usr/bin/env bash

rm -rf build_emscripten
source ../emsdk/emsdk_env.sh
mkdir build_emscripten
cd build_emscripten
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 16
cp ../_headers .
mv test_scrolling_pane.html index.html
rm -rf CMakeFiles
rm -rf externals
rm -rf tmp
rm CMakeCache.txt
rm cmake_install.cmake
rm compile_commands.json
rm Makefile
