/*

Copyright 2011-2018 Stratify Labs, Inc

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

 */


#include <sys/lock.h>
#include <fcntl.h>
#include <errno.h>
#include <mcu/mcu.h>
#include <mcu/appfs.h>
#include <mcu/debug.h>
#include <mcu/periph.h>
#include <device/sys.h>
#include <device/uartfifo.h>
#include <device/usbfifo.h>
#include <device/fifo.h>
#include <device/cfifo.h>
#include <device/sys.h>
#include <sos/link.h>
#include <sos/fs/sysfs.h>
#include <sos/fs/appfs.h>
#include <sos/fs/devfs.h>
#include <sos/fs/sffs.h>
#include <sos/fs/fatfs.h>
#include <sos/sos.h>
#include <sos/fs/assetfs.h>

#include "config.h"
#include "sl_config.h"
#include "link_config.h"
#include "link_config_uart.h"
#include "ram_device.h"
#include "ramdisk.h"

//--------------------------------------------Stratify OS Configuration-------------------------------------------------
const sos_board_config_t sos_board_config = {
	.clk_usecond_tmr = SOS_BOARD_TMR, //TIM2 -- 32 bit timer
	.task_total = SOS_BOARD_TASK_TOTAL,
	.stdin_dev = "/dev/stdio-in" ,
	.stdout_dev = "/dev/stdio-out",
	.stderr_dev = "/dev/stdio-out",
	.o_sys_flags = SYS_FLAG_IS_STDIO_FIFO | SYS_FLAG_IS_TRACE,
	.sys_name = SL_CONFIG_NAME,
	.sys_version = SL_CONFIG_VERSION_STRING,
	.sys_id = SL_CONFIG_DOCUMENT_ID,
	.team_id = SL_CONFIG_TEAM_ID,
	.sys_memory_size = SOS_BOARD_SYSTEM_MEMORY_SIZE,
	.start = sos_default_thread,
	.start_args = &link_transport,
	.start_stack_size = 4096,
	.socket_api = 0,
	.request = 0,
	.trace_dev = "/dev/trace",
	.trace_event = SOS_BOARD_TRACE_EVENT,
	.git_hash = SOS_GIT_HASH
};

//This declares the task tables required by Stratify OS for applications and threads
SOS_DECLARE_TASK_TABLE(SOS_BOARD_TASK_TOTAL);


//--------------------------------------------Root Filesystem---------------------------------------------------

/*
 * This is the root filesystem that determines what is mounted at /.
 *
 * The default is /app (for installing and running applciations in RAM and flash) and /dev which
 * provides the device tree defined above.
 *
 * Additional filesystems (such as FatFs) can be added if the hardware and drivers
 * are provided by the board.
 *
 */

//#if !defined BOOTLOADER_MODE
#if 1
//The bootloader won't be running apps so it doesn't need /app

u32 ram_usage_table[APPFS_RAM_USAGE_WORDS(APPFS_RAM_PAGES)] MCU_SYS_MEM;

const appfs_mem_config_t appfs_mem_config = {
	.usage_size = sizeof(ram_usage_table),
	.usage = ram_usage_table,
	.system_ram_page = (u32)-1, //system RAM is not located in the APPFS memory sections
	.flash_driver = 0,
	.section_count = 1,
	.sections = {
		{ .o_flags = MEM_FLAG_IS_RAM, .page_count = APPFS_RAM_PAGES, .page_size = MCU_RAM_PAGE_SIZE, .address = 0x80000000 }
		//{ .o_flags = MEM_FLAG_IS_RAM, .page_count = 128, .page_size = MCU_RAM_PAGE_SIZE, .address = 0x20200000 }
	}
};

const devfs_device_t mem0 = DEVFS_DEVICE("mem0", appfs_mem, 0, &appfs_mem_config, 0, 0666, SOS_USER_ROOT, S_IFBLK);

FATFS_DECLARE_CONFIG_STATE(fatfs, (sysfs_list + 1), "drive0", 0, 0, 0);
#endif


#if 0
ASSETFS_FILE(map, "../build_flexspi_release/MIMXRT1050-EVK_flexspi_release.map");

const assetfs_config_t asset_config = {
	.count = 1,
	.entries = {
		ASSETFS_ENTRY("map.map", map, 0666)
	}
};
#endif


//const devfs_device_t mem0 = DEVFS_DEVICE("mem0", mcu_mem, 0, 0, 0, 0666, SOS_USER_ROOT, S_IFBLK);
const sysfs_t sysfs_list[] = {
//#if !defined BOOTLOADER_MODE
#if 1
	APPFS_MOUNT("/app", &mem0, 0777, SYSFS_ROOT), //the folder for ram/flash applications
#endif
	DEVFS_MOUNT("/dev", devfs_list, 0777, SYSFS_ROOT), //the list of devices
#if 0
	ASSETFS_MOUNT("/assets", &asset_config, 0777, SYSFS_ROOT),
#endif
	FATFS_MOUNT("/tmp", &fatfs_config, 0777, SYSFS_ROOT),
	SYSFS_MOUNT("/", sysfs_list, 0777, SYSFS_ROOT), //the root filesystem (must be last)
	SYSFS_TERMINATOR
};

