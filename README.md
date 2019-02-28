# MIMXRT1050-EVK

Stratify OS board support package for the MIMXRT1050-EVK


## Development Environment Setup

### Prerequisites

- git (Mac, type `git --version` in Terminal to install) or [Git Bash](https://gitforwindows.org/) (Windows)
- cmake (using `sl sdk.install:cmake` or install from https://cmake.org/download/)

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
sl --update # follow instructions if there is an update ready
sl sdk.install # installs the SDK
# the next commands configure the workspace (current working directory) to build and pull latest libraries
sl sdk.update:library=StratifyOS,cmo='-DSOS_SKIP_CMAKE=OFF -DBUILD_ALL=ON -DBUILD_ARM_ALL=OFF -DBUILD_ARM_V7EM_F5DH=ON'
sl sdk.update:library=StratifyOS-mcu-imxrt
sl sdk.update:library=son?sgfx,cmo='-DBUILD_ARM_ALL=OFF -DBUILD_ARM_V7EM_F5DH=ON' # optional for application development
sl sdk.update:library=StratifyAPI,cmo='-DBUILD_ARM_ALL=OFF -DBUILD_ARM_V7EM_F5DH=ON' # optional for application development
sl sdk.update # actually does the pulling and building
```

Subsequently, the SDK can be updated with `sl sdk.update` called in the same workspace directory as the commands above.

### Build and Flash

```
git clone https://github.com/StratifyLabs/MIMXRT1050-EVK.git
sl os.build:path=MIMXRT1050-EVK
```

Now you need to copy the file `MIMXRT1050-EVK/build_flexspi_debug/MIMXRT1050-EVK.bin`
to the RT1050-EVK mbed drive mount and then hit the reset button.

### Expected Output

If you monitor the UART output, you will see the system booting up. You will
also see the green light turn on and off after each time the reset button is pushed.

```
INFO:SYS:MCU Debug start
INFO:MALLOC:Get more memory
INFO:MALLOC:a:0 0x200106B4 2048 0x20010008 0x20010008
INFO:SYS:Enter default thread
INFO:SYS:init /dev
INFO:SYS:init /
INFO:SYS:Started Filesystem Apps 0
INFO:SYS:Open RTC
WARN:SYS:RTC not opened
INFO:LINK:Open link driver
INFO:LINK:start link update
```


Ping using `sl`


```
sl conn.connect:baudrate=115200

- connection.connect:
	 options:
			retry: 5
			delay: 500
			path: <any>
			serialNumber: <any>
			parity: none
			baudrate: 115200
			stopbits: 1
	 output:
			system.information:
				 path: /dev/tty.usbmodem1412301
				 name: MIMXRT1050-EVK
				 serialNumber: 0000000065F823AC65F823AC65F823AC
				 hardwareId: 00000024
				 projectId: <invalid>
				 bspVersion: 0.1
				 sosVersion: 3.8.0db
				 cpuArchitecture: v7em_f5dh
				 cpuFreqency: 528000000
				 applicationSignature: 0
				 bspGitHash: df4f62e
				 sosGitHash: e0ed670
				 mcuGitHash: 4c2dd32
	 result: success

```

View the /dev folder

```
sl conn.connect:baudrate=115200 fs.list:path=/dev
```

## Debugging

Connect using openOCD.

```
openocd -f interface/cmsis-dap.cfg -f ./MIMXRT1050-EVK/imxrt.cfg
```
