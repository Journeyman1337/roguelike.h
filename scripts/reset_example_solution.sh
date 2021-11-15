cd ..

git submodule deinit -f -- ./vcpkg
rm -rf .git/modules/vcpkg
git rm -f ./vcpkg

rm -rf ./build/