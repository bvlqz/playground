./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh


rm -rf build
mkdir build
cd build
emcmake cmake -G Ninja ..
emmake ninja


