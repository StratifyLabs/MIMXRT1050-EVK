# MIMXRT1050-EVK
Stratify OS board support package for the MIMXRT1050-EVK


```
git clone https://github.com/StratifyLabs/StratifySDK.git
cd StratifySDK
cmake -P StratifySDK.cmake
```

Build https://github.com/StratifyLabs/StratifyOS-mcu-imxrt library.


```
git clone https://github.com/StratifyLabs/MIMXRT1050-EVK.git
cd MIMXRT1050-EVK
mkdir cmake_arm
cd cmake_arm
cmake ..
cmake --build . --target size_debug -- -j 8
```
