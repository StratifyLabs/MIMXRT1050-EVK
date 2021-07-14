cmake_minimum_required (VERSION 3.6)

# Grab the SDK location
if(NOT DEFINED SOS_SDK_PATH)
	if(DEFINED ENV{SOS_SDK_PATH})
		set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH /Applications/StratifyLabs-SDK)
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH C:/StratifyLabs-SDK)
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH /StratifyLabs-SDK)
	endif()
endif()
if(NOT DEFINED SOS_SDK_PATH)
	message(FATAL_ERROR "Please set environment variable SOS_SDK_PATH to location of the StratifyLabs-SDK directory")
endif()
set(SOS_TOOLCHAIN_CMAKE_PATH ${SOS_SDK_PATH}/Tools/gcc/arm-none-eabi/cmake)

include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-variables.cmake)
set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp-toolchain.cmake)
get_filename_component(SOS_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/src)
file(GLOB LINKER_SCRIPTS)
add_subdirectory(src)
list(APPEND KERNEL_SOURCELIST
	${SOURCES}
	imxrt.cfg
  ${CMAKE_SOURCE_DIR}/README.md
  ${CMAKE_SOURCE_DIR}/sl_settings.json
  )

#Change to boot sources
set(SOURCES "")
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/boot)
add_subdirectory(boot)
list(APPEND BOOT_SOURCELIST ${SOURCES})

set(SOS_ARCH v7em_f5dh)
set(SOS_DEVICE imxrt1052ram)
set(SOS_DEVICE_FAMILY imxrt1052)
set(SOS_HARDWARD_ID 0x00000026)
set(SOS_LIBRARIES son_kernel sos_mcu_imxrt1052 fatfs_kernel)
set(SOS_INCLUDE_DIRECTORIES ${TOOLCHAIN_INC_DIR}/mcu/arch/imxrt/cmsis)
set(SOS_DEFINITIONS CPU_MIMXRT1052CVJ5B=1)


set(SOS_START_ADDRESS 0x0)
set(SOS_SOURCELIST ${KERNEL_SOURCELIST})
set(SOS_CONFIG release)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)
set(SOS_LIBRARIES son_kernel sos_mcu_imxrt1052_debug fatfs_kernel_debug)
set(SOS_CONFIG debug)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)

set(SOS_START_ADDRESS 0x60002000)
set(SOS_DEVICE imxrt1052flexspi)
set(SOS_BUILD_FLAGS -fno-delete-null-pointer-checks)
set(SOS_DEFINITIONS ${SOS_DEFINITIONS} BOOTLOADER_MODE=1)

set(SOS_SOURCELIST ${KERNEL_SOURCELIST})
set(SOS_CONFIG flexspi_release)
set(SOS_LIBRARIES son_kernel sos_mcu_imxrt1052 fatfs_kernel)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)
set(SOS_CONFIG flexspi_debug)
set(SOS_LIBRARIES son_kernel sos_mcu_imxrt1052_debug fatfs_kernel_debug)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)

