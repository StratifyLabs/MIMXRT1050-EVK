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


#include "sos/sos.h"

#include <errno.h>
#include <unistd.h>
#include "config.h"
#include "ramdisk.h"
#include "cortexm/cortexm.h"
#include "mcu/debug.h"

#include "mcu/core.h"
#include "cortexm/task.h"


int ramdisk_open(const devfs_handle_t * handle){
    mcu_debug_log_info(MCU_DEBUG_SYS, "%s\n", __func__);
    return 0;
}

int ramdisk_read(const devfs_handle_t * handle, devfs_async_t * async){
    const ramdisk_config_t *config = handle->config;
    if( async->loc <  0 ){ return SYSFS_SET_RETURN(EINVAL); }
    if( (async->loc + async->nbyte) >= (config->page_size * config->page_cnt) ) {
      return SYSFS_SET_RETURN(EINVAL);
    }
    async->loc += config->start;

    mcu_debug_log_info(MCU_DEBUG_SYS, "%s loc= %0x nbyte= %d\n", __func__, async->loc, async->nbyte);

    memcpy(async->buf, (void*)async->loc, async->nbyte);

    if (async->nbyte >= 512) {
      char * p = (char *)async->loc;
      mcu_debug_log_info (MCU_DEBUG_SYS, "%s [0]= %0x [1]= %0x [510]= %0x [511]= %0x\n", __func__,
          p[0], p[1], p[510], p[511]);
    }

    return async->nbyte;
}

int ramdisk_write(const devfs_handle_t * handle, devfs_async_t * async){
    const ramdisk_config_t *config = handle->config;
    if( async->loc <  0 ){ return SYSFS_SET_RETURN(EINVAL); }
    if( (async->loc + async->nbyte) >= (config->page_size * config->page_cnt) ) {
      return SYSFS_SET_RETURN(EINVAL);
    }
    async->loc += config->start;

    mcu_debug_log_info(MCU_DEBUG_SYS, "%s loc= %0x nbyte= %d\n", __func__, async->loc, async->nbyte);
    
    memcpy((void*)async->loc, async->buf, async->nbyte);

    if (async->nbyte >= 512) {
      char * p = (char *)async->loc;
      mcu_debug_log_info (MCU_DEBUG_SYS, "%s [0]= %0x [1]= %0x [510]= %0x [511]= %0x\n", __func__,
          p[0], p[1], p[510], p[511]);
    }

    return async->nbyte;
}

int ramdisk_ioctl(const devfs_handle_t * handle, int request, void * ctl){
    const ramdisk_config_t *config = handle->config;
    drive_info_t * info = ctl;
    drive_attr_t * attr = ctl;
    u32 o_flags;

    switch(request){
    case I_DRIVE_SETATTR:
        o_flags = attr->o_flags;
        if( o_flags & (DRIVE_FLAG_ERASE_BLOCKS | DRIVE_FLAG_ERASE_DEVICE) ){

            int s = config->start;
            size_t n = config->page_size * config->page_cnt;
            if( o_flags & DRIVE_FLAG_ERASE_BLOCKS ){
              s = attr->start + config->start;
              n = attr->end - attr->start;
            }

            if (0 == memset((void *)s, 0, n)) {
              mcu_debug_log_info(MCU_DEBUG_SYS, "%s SETATTR ERASE 0\n", __func__);
              return 0;
            }

            mcu_debug_log_info(MCU_DEBUG_SYS, "%s SETATTR ERASE EIO\n", __func__);
            return SYSFS_SET_RETURN(EIO);
        }

        if( o_flags & DRIVE_FLAG_INIT ){
            // nothing to do upon init
            mcu_debug_log_info(MCU_DEBUG_SYS, "%s SETATTR INIT 0\n", __func__);
            return 0;
        }

        break;

    case I_DRIVE_ISBUSY:
        //mcu_debug_log_info(MCU_DEBUG_SYS, "%s ISBUSY\n", __func__);
        return 0;

    case I_DRIVE_GETINFO:
        info->o_flags = DRIVE_FLAG_ERASE_BLOCKS | DRIVE_FLAG_ERASE_DEVICE | DRIVE_FLAG_INIT;
        info->o_events = 0;
        info->address_size = 1;
        info->bitrate = SOS_BOARD_SYSTEM_CLOCK;
        info->erase_block_size = 1;
        info->erase_block_time = 0;
        info->erase_device_time = 0;
        info->num_write_blocks = config->page_cnt * config->page_size;
        info->write_block_size = 1;
        mcu_debug_log_info(MCU_DEBUG_SYS, "%s GETINFO\n", __func__);
        break;

    default:
        mcu_debug_log_info(MCU_DEBUG_SYS, "%s default EINVAL\n", __func__);
        return SYSFS_SET_RETURN(EINVAL);
    }
    return 0;
}

int ramdisk_close(const devfs_handle_t * handle){
    mcu_debug_log_info(MCU_DEBUG_SYS, "%s\n", __func__);
    return 0;
}

