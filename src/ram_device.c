/*

Copyright 2011-2019 Stratify Labs, Inc

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

#include <mcu/debug.h>
#include <string.h>
#include <cortexm/task.h>
#include <sos/fs/devfs.h>
#include "ram_device.h"

extern int signal_root_send(int send_tid,
		int tid,
		int si_signo,
		int si_sigcode,
		int sig_value,
		int forward //this must be non-zero unless si_signo == SIGKILL
		);

int ram_device_open(const devfs_handle_t * handle){
	MCU_UNUSED_ARGUMENT(handle);
	return 0;
}

int ram_device_write(const devfs_handle_t * handle, devfs_async_t * async){
	MCU_UNUSED_ARGUMENT(handle);
	if( async->loc <  0 ){ return SYSFS_SET_RETURN(EINVAL); }
	if( async->loc + async->nbyte >= 256*1024 ){ return SYSFS_SET_RETURN(EINVAL); }
	memcpy((void*)async->loc, async->buf, async->nbyte);
	return async->nbyte;
}

int ram_device_read(const devfs_handle_t * handle, devfs_async_t * async){
	MCU_UNUSED_ARGUMENT(handle);
	MCU_UNUSED_ARGUMENT(async);
	return SYSFS_SET_RETURN(ENOTSUP);
}

int ram_device_ioctl(const devfs_handle_t * handle, int request, void * ctl){
	MCU_UNUSED_ARGUMENT(handle);
	MCU_UNUSED_ARGUMENT(request);
	MCU_UNUSED_ARGUMENT(ctl);
	return SYSFS_SET_RETURN(ENOTSUP);
}

int ram_device_close(const devfs_handle_t * handle){
	MCU_UNUSED_ARGUMENT(handle);

	//after RAM device is closed sl can send an ALARM signal to invoke the new image


	return 0;
}

