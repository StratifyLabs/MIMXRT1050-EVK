cmake_minimum_required (VERSION 3.6)

#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/src)
file(GLOB LINKER_SCRIPTS)
add_subdirectory(src)
list(APPEND KERNEL_SOURCELIST
	${SOURCES}
	imxrt.cfg
	${CMAKE_SOURCE_DIR}/README.md
	)

#Change to boot sources
set(SOURCES "")
set(SOURCES_PREFIX ${CMAKE_SOURCE_DIR}/boot)
add_subdirectory(boot)
list(APPEND BOOT_SOURCELIST ${SOURCES})

if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
  set(SOS_TOOLCHAIN_CMAKE_PATH /Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
	set(SOS_TOOLCHAIN_CMAKE_PATH C:/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp-toolchain.cmake)
get_filename_component(SOS_BSP_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_BSP_NAME} CXX C)

set(SOS_BSP_ARCH v7em_f5dh)
set(SOS_BSP_DEVICE imxrt1052ram)
set(SOS_BSP_DEVICE_FAMILY imxrt1052)
set(SOS_BSP_HARDWARD_ID 0x00000024)
set(SOS_BSP_LIBRARIES son_kernel sos_mcu_imxrt1052)
set(SOS_BSP_INCLUDE_DIRECTORIES ${TOOLCHAIN_INC_DIR}/mcu/arch/imxrt/cmsis)
set(SOS_BSP_DEFINITIONS CPU_MIMXRT1052CVJ5B=1)


set(SOS_BSP_START_ADDRESS 0x0)
set(SOS_BSP_SOURCELIST ${KERNEL_SOURCELIST})
set(SOS_BSP_CONFIG release)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)
set(SOS_BSP_LIBRARIES son_kernel sos_mcu_imxrt1052_debug)
set(SOS_BSP_CONFIG debug)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)

set(SOS_BSP_START_ADDRESS 0x60002000)
set(SOS_BSP_DEVICE imxrt1052flexspi)
set(SOS_BSP_SOURCELIST ${KERNEL_SOURCELIST})
set(SOS_BSP_CONFIG flexspi_release)
set(SOS_BSP_LIBRARIES son_kernel sos_mcu_imxrt1052)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)
set(SOS_BSP_CONFIG flexspi_debug)
set(SOS_BSP_LIBRARIES son_kernel sos_mcu_imxrt1052_debug)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp.cmake)

