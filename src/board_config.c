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

#include <string.h>
#include <fcntl.h>
#include <sos/sos.h>
#include <mcu/debug.h>
#include <cortexm/task.h>
#include <sos/link/types.h>
#include <mcu/bootloader.h>
#include <mcu/tmr.h>
#include <cortexm/mpu.h>

#include <mcu/arch/imxrt/mimxrt1052/fsl_common.h>
#include <mcu/arch/imxrt/mimxrt1052/fsl_iomuxc.h>
#include <mcu/arch/imxrt/mimxrt1052/fsl_flexram.h>
#include <mcu/arch/imxrt/mimxrt1052/fsl_gpio.h>

#include "config.h"
#include "board_config.h"
#include "link_config_uart.h"

#define TRACE_COUNT 8
#define TRACE_FRAME_SIZE sizeof(link_trace_event_t)
#define TRACE_BUFFER_SIZE (sizeof(link_trace_event_t)*TRACE_COUNT)
static char trace_buffer[TRACE_FRAME_SIZE*TRACE_COUNT];
const ffifo_config_t board_trace_config = {
	.frame_count = TRACE_COUNT,
	.frame_size = sizeof(link_trace_event_t),
	.buffer = trace_buffer
};
ffifo_state_t board_trace_state;

extern void SystemClock_Config();

void board_trace_event(void * event){
	link_trace_event_header_t * header = event;
	devfs_async_t async;
	const devfs_device_t * trace_dev = &(devfs_list[0]);

	//write the event to the fifo
	memset(&async, 0, sizeof(devfs_async_t));
	async.tid = task_get_current();
	async.buf = event;
	async.nbyte = header->size;
	async.flags = O_RDWR;
	trace_dev->driver.write(&(trace_dev->handle), &async);
}

#if defined BOOTLOADER_MODE

const bootloader_board_config_t boot_board_config = {
	.sw_req_loc = 0x10002000, //needs to reside in RAM that is preserved through reset and available to both bootloader and OS
	.sw_req_value = 0x55AA55AA, //this can be any value
	.program_start_addr = 0x0, //RAM image starts here
	.hw_req.port = 0xff, .hw_req.pin = 0xff,
	.o_flags = 0,
	.link_transport_driver = 0,
	.id = 1,
};

static void * stack_ptr;
static void (*app_reset)();
#define SYSTICK_CTRL_TICKINT (1<<1)

static void handle_alarm(int signo){
	mcu_debug_printf("signal %d\n", signo);

	sleep(1); //wait one second to allow the sl command to complete

	//link_transport_uart_close(&link_transport_uart.handle);
	cortexm_svcall((cortexm_svcall_t)cortexm_set_privileged_mode, 0);

	//shutdown the system timer
	devfs_handle_t tmr_handle;
	tmr_handle.port = SOS_BOARD_TMR;
	mcu_tmr_close(&tmr_handle);

	u32 * start_of_program = (u32*)(boot_board_config.program_start_addr);
	stack_ptr = (void*)start_of_program[0];
	app_reset = (void (*)())( start_of_program[1] );

	//turn off MPU, disable interrupts and SYSTICK
	cortexm_reset_mode();

	//assign the value of the MSP based on the RAM image, currently the PSP is in use so it can be a call
	cortexm_set_stack_ptr(stack_ptr);

	/*
	 * Stop using the PSP. This must be inlined. If a call was made,
	 * the return would cause problems because lr would be popped
	 * from the wrong stack.
	 */
	register u32 control;
	control = __get_CONTROL();
	control &= ~0x03;
	__set_CONTROL(control);

	/*
	 * This will start executing the OS that is currently in RAM.
	 *
	 */
	app_reset();

	//should never get here
	sos_led_root_error(0);
}

static const flexram_allocate_ram_t flexram_config = {
	.ocramBankNum = 4, //128K
	.dtcmBankNum = 4, //128K
	.itcmBankNum = 8 //256K
};

#endif


void board_event_handler(int event, void * args){
	switch(event){
		case MCU_BOARD_CONFIG_EVENT_ROOT_TASK_INIT:
			break;

		case MCU_BOARD_CONFIG_EVENT_ROOT_FATAL:
			cortexm_disable_interrupts();
			//start the bootloader on a fatal event
			//mcu_core_invokebootloader(0, 0);
			if( args != 0 ){
				mcu_debug_log_error(MCU_DEBUG_SYS, "Fatal Error %s", (const char*)args);
			} else {
				mcu_debug_log_error(MCU_DEBUG_SYS, "Fatal Error unknown");
			}

			sos_led_root_error(0);
			while(1){
				;
			}
			break;

		case MCU_BOARD_CONFIG_EVENT_ROOT_INITIALIZE_CLOCK:

			//configure the IO pins -- functionality needs to move to mcu_pio_setattr()

#if defined BOOTLOADER_MODE

			CLOCK_EnableClock(kCLOCK_Iomuxc);          /* iomuxc clock (iomuxc_clk_enable): 0x03u */

			//LED
			IOMUXC_SetPinMux(
						IOMUXC_GPIO_AD_B0_09_GPIO1_IO09,
						0);

			IOMUXC_SetPinConfig(
						IOMUXC_GPIO_AD_B0_09_GPIO1_IO09,
						IOMUXC_SW_PAD_CTL_PAD_DSE(7));

			IOMUXC_SetPinMux(
						IOMUXC_GPIO_AD_B0_12_LPUART1_TX,        /* GPIO_AD_B0_12 is configured as LPUART1_TX */
						0U);                                    /* Software Input On Field: Input Path is determined by functionality */
			IOMUXC_SetPinMux(
						IOMUXC_GPIO_AD_B0_13_LPUART1_RX,        /* GPIO_AD_B0_13 is configured as LPUART1_RX */
						0U);                                    /* Software Input On Field: Input Path is determined by functionality */
			IOMUXC_SetPinConfig(
						IOMUXC_GPIO_AD_B0_12_LPUART1_TX,        /* GPIO_AD_B0_12 PAD functional properties : */
						0x10B0u);                               /* Slew Rate Field: Slow Slew Rate
																		  Drive Strength Field: R0/6
																		  Speed Field: medium(100MHz)
																		  Open Drain Enable Field: Open Drain Disabled
																		  Pull / Keep Enable Field: Pull/Keeper Enabled
																		  Pull / Keep Select Field: Keeper
																		  Pull Up / Down Config. Field: 100K Ohm Pull Down
																		  Hyst. Enable Field: Hysteresis Disabled */
			IOMUXC_SetPinConfig(
						IOMUXC_GPIO_AD_B0_13_LPUART1_RX,        /* GPIO_AD_B0_13 PAD functional properties : */
						0x10B0u);                               /* Slew Rate Field: Slow Slew Rate
																		  Drive Strength Field: R0/6
																		  Speed Field: medium(100MHz)
																		  Open Drain Enable Field: Open Drain Disabled
																		  Pull / Keep Enable Field: Pull/Keeper Enabled
																		  Pull / Keep Select Field: Keeper
																		  Pull Up / Down Config. Field: 100K Ohm Pull Down
																		  Hyst. Enable Field: Hysteresis Disabled */

			IOMUXC_SetPinMux(
						IOMUXC_GPIO_AD_B1_06_LPUART3_TX,
						0U);
			IOMUXC_SetPinMux(
						IOMUXC_GPIO_AD_B1_07_LPUART3_RX,
						0U);
			IOMUXC_SetPinConfig(
						IOMUXC_GPIO_AD_B1_06_LPUART3_TX,
						0x10B0u);
			IOMUXC_SetPinConfig(
						IOMUXC_GPIO_AD_B1_07_LPUART3_RX,
						0x10B0u);

			FLEXRAM_AllocateRam((flexram_allocate_ram_t*)&flexram_config);
			SystemClock_Config();

			if( GPIO_PinRead(GPIO5, 0) == 0 ){
				sos_led_root_error(0);
			}
#else
			//OS mode
			SystemClock_Config();
#endif

			break;

		case MCU_BOARD_CONFIG_EVENT_START_INIT:

			//handle alarm by resetting the device -- ram device sends the signal when an image has been installed
#if defined  BOOTLOADER_MODE
			mcu_debug_printf("SIGALRM activated for bootloader mode\n");
			signal(SIGALRM, handle_alarm);
#else
			mcu_debug_log_info(MCU_DEBUG_USER1, "Executing RAM image");
#endif


			break;

		case MCU_BOARD_CONFIG_EVENT_START_LINK:
			mcu_debug_log_info(MCU_DEBUG_USER1, "Format /tmp");
			if( mkfs("/tmp") < 0 ){
				mcu_debug_log_info(MCU_DEBUG_USER1, "failed to format /tmp");
			} else {
				if( mount("/tmp") < 0 ){
					mcu_debug_log_info(MCU_DEBUG_USER1, "failed to mount /tmp");
				}
			}

			mcu_debug_log_info(MCU_DEBUG_USER1, "Start LED %d");
			sos_led_startup();
			break;

		case MCU_BOARD_CONFIG_EVENT_START_FILESYSTEM:
			break;
	}
}
