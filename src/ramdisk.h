/* Copyright 2019 Brandon Anderson
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include "sos/fs/devfs.h"
#include "sos/dev/drive.h"


typedef struct {
    int start;
    size_t page_size;
    int page_cnt;
} ramdisk_config_t;


int ramdisk_open(const devfs_handle_t * handle);
int ramdisk_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int ramdisk_read(const devfs_handle_t * handle, devfs_async_t * async);
int ramdisk_write(const devfs_handle_t * handle, devfs_async_t * async);
int ramdisk_close(const devfs_handle_t * handle);


#endif /* RAMDISK_H_ */
