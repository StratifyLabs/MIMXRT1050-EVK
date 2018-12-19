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


#include <mcu/types.h>
#include <mcu/core.h>
#include <mcu/bootloader.h>
#include <mcu/arch.h>

#include "boot_link_config.h"

const struct __sFILE_fake __sf_fake_stdin;
const struct __sFILE_fake __sf_fake_stdout;
const struct __sFILE_fake __sf_fake_stderr;

#include "../src/config.h"
//Bootloader configuration (don't need Stratify OS configuration for just the bootloader)
//STM32_NUCLEO144_DECLARE_BOOT_BOARD_CONFIG(&link_transport);

extern void SystemClock_Config();
extern void boot_main();

const bootloader_board_config_t boot_board_config = {
		.sw_req_loc = 0x10002000,
		.sw_req_value = 0x55AA55AA,
		.program_start_addr = 0x40000,
		.hw_req.port = 0, .hw_req.pin = 16, //p14 on MBED, center joystick on xively application board
		.o_flags = BOOT_BOARD_CONFIG_FLAG_HW_REQ_ACTIVE_HIGH | BOOT_BOARD_CONFIG_FLAG_HW_REQ_PULLDOWN,
		.link_transport_driver = &link_transport,
		.id = __HARDWARE_ID
};

//Execute the Stratify OS default bootloader
void _main(){ boot_main(); }

void board_event_handler(int event, void * args){
    switch(event){
    case MCU_BOARD_CONFIG_EVENT_ROOT_INITIALIZE_CLOCK:
        SystemClock_Config();
        break;
    }
}
