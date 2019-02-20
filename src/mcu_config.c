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

#include <mcu/arch.h>
#include <mcu/mcu.h>

#include "config.h"

//--------------------------------------------MCU Configuration-------------------------------------------------


const mcu_board_config_t mcu_board_config = {
	.core_osc_freq = SOS_BOARD_SYSTEM_CLOCK,
	.core_cpu_freq = SOS_BOARD_SYSTEM_CLOCK,
	.core_periph_freq = SOS_BOARD_SYSTEM_CLOCK,
	.usb_max_packet_zero = 64,
	.debug_uart_port = 0,
	.debug_uart_attr = {
		.pin_assignment =
		{
			.rx = {0, 2},
			.tx = {0, 3},
			.cts = {0xff, 0xff},
			.rts = {0xff, 0xff}
		},
		.freq = 115200,
		.o_flags = UART_FLAG_IS_PARITY_NONE | UART_FLAG_IS_STOP1,
		.width = 8
	},
	.o_flags = 0,
	.event_handler = board_event_handler,
	.led = {0, 9}, //GPIO_AD_B0_09 is GPIO1_IO09 (port 0 pin 9)
	.o_mcu_debug = MCU_DEBUG_INFO | MCU_DEBUG_SYS
};
