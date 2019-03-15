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

#ifndef RAM_DEVICE_H_
#define RAM_DEVICE_H_

#include <sos/fs/types.h>

int ram_device_open(const devfs_handle_t * handle);
int ram_device_write(const devfs_handle_t * handle, devfs_async_t * async);
int ram_device_read(const devfs_handle_t * handle, devfs_async_t * async);
int ram_device_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int ram_device_close(const devfs_handle_t * handle);

#endif /* RAM_DEVICE_H_ */
