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


```
openocd -f interface/cmsis-dap.cfg -f ./MIMXRT1050-EVK/imxrt.cfg


telnet localhost 4444
load_image ./MIMXRT1050-EVK/build_debug/MIMXRT1050-EVK.bin 0 bin
```


Default RAM config is 128K ITCM 128K DTCM and 256K OCRAM (all 512KB seem to be availabe in OCRAM)


SCB->VTOR = 0xE000E000UL + 0x0D00UL + 8 = 0xE000ED08

```
reset halt
load_image ./MIMXRT1050-EVK/build_debug/MIMXRT1050-EVK.bin 0 bin
reg msp 0x20010000
mww 0xE000ED08 0
resume 1

rbp 0x3108
bp 0x3e3c 2
resume 1


cortex_m reset_config vectreset

rbp 0x3c72
