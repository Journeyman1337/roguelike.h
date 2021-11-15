cd ..
git submodule add https://github.com/microsoft/vcpkg vcpkg/
pushd vcpkg
./bootstrap-vcpkg.sh -disableMetrics
popd
cmake -S . -B ./build/ -DRLH_BUILD_EXAMPLES=ON