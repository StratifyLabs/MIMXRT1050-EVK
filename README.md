# MIMXRT1050-EVK

Stratify OS board support package for the MIMXRT1050-EVK


## Development Environment Setup

To develop device drivers and to customize this OS package you will need to
run the following commands in a Bash shell (Windows Git Bash or Mac OS X Terminal)

- Install `sl` command line tool (see https://app.stratifylabs.co for more details)
- Install the SDK. Pull latest libraries needed for imxrt development
- Build and Flash the OS package


### Install `sl` command line Tool on Mac

```
mkdir -p /Applications/StratifyLabs-SDK/Tools/gcc/bin
curl -L -o /Applications/StratifyLabs-SDK/Tools/gcc/bin/sl 'https://stratifylabs.page.link/slmac'
chmod 755 /Applications/StratifyLabs-SDK/Tools/gcc/bin/sl
echo 'export PATH=/Applications/StratifyLabs-SDK/Tools/gcc/bin:$PATH' >> ~/.bash_profile
source ~/.bash_profile
```

### Install `sl` command line Tool on Mac
```
mkdir -p /C/StratifyLabs-SDK/Tools/gcc/bin
curl -L -o /C/StratifyLabs-SDK/Tools/gcc/bin/sl.exe 'https://stratifylabs.page.link/slwin'
chmod 755 /C/StratifyLabs-SDK/Tools/gcc/bin/sl.exe
echo 'export PATH=/C/StratifyLabs-SDK/Tools/gcc/bin:$PATH' >> ~/.bash_profile
source ~/.bash_profile
```

### Install the SDK and pull latest libraries

```
sl sdk.install # installs the SDK
# the next commands configure the workspace (current working directory) to build and pull latest libraries
sl sdk.update:library=StratifyOS,cmo='-DSOS_SKIP_CMAKE=OFF -DBUILD_ALL=ON -DBUILD_ARM_ALL=OFF -DBUILD_ARM_V7EM_F5SH=ON'
sl sdk.update:library=StratifyOS-mcu-imxrt
sl sdk.uddate:library=StratifyAPI,cmo='-DBUILD_ARM_ALL=OFF -DBUILD_ARM_V7EM_F5SH=ON'
sl sdk.update #actually does the pulling and building
```

### Build and Flash

```
git clone https://github.com/StratifyLabs/MIMXRT1050-EVK.git
sl os.build:path=MIMXRT1050-EVK
```

Now you need to copy the file MIMXRT1050-EVK/build_flexspi_debug/MIMXRT1050-EVK.bin
to the RT1050-EVK mbed drive mount and then hit the reset button.

If you monitor the UART output, you will see the system booting up. You will
also see the green light turn on and off after each time the reset button is pushed.
