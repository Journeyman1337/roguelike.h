#!/bin/bash
git submodule add https://github.com/microsoft/vcpkg vcpkg/
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics
cd ..
cmake -S . -B ./build/ -DRLH_BUILD_EXAMPLES=ON
