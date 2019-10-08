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
    mcu_debug_log_info(MCU_DEBUG_SYS, "%s", __func__);
    return 0;
}

int ramdisk_read(const devfs_handle_t * handle, devfs_async_t * async){
    const ramdisk_config_t *config = handle->config;
    if( async->loc <  0 ){ return SYSFS_SET_RETURN(EINVAL); }
    if( (async->loc + async->nbyte) >= (config->page_size * config->page_cnt) ) {
      return SYSFS_SET_RETURN(EINVAL);
    }
    uint16_t *sdram = (uint16_t *)((async->loc * 512) + config->start);
    uint16_t *dest = async->buf;

    mcu_debug_log_info(MCU_DEBUG_SYS, "%s loc= %p nbyte= %d", __func__, sdram, async->nbyte);

    // use 16-bit interface on EVKB
    int len = async->nbyte / 2; // localize our loop endpoint for thread/ISR safety
    mcu_core_invalidate_data_cache();
    for(int i = 0; i < len; i++) {
      dest[i] = sdram[i];
    }
    if (len % 2 > 0) {
      mcu_debug_log_error(MCU_DEBUG_SYS, "%s loc= %p nbyte= %d odd not handled\n", __func__, sdram, async->nbyte);
      //FIXME
    }

    if (async->nbyte >= 512) {
      mcu_debug_log_info (MCU_DEBUG_SYS, "%s [w0]= %0x [w255]= %0x", __func__,
          sdram[0], sdram[255]);
    }

    return async->nbyte;
}

int ramdisk_write(const devfs_handle_t * handle, devfs_async_t * async){
    const ramdisk_config_t *config = handle->config;
    if( async->loc <  0 ){ return SYSFS_SET_RETURN(EINVAL); }
    if( (async->loc + async->nbyte) >= (config->page_size * config->page_cnt) ) {
      return SYSFS_SET_RETURN(EINVAL);
    }
    uint16_t *sdram = (uint16_t *)((async->loc * 512) + config->start);
    uint16_t *src = async->buf;

    mcu_debug_log_info(MCU_DEBUG_SYS, "%s loc= %p nbyte= %d", __func__, sdram, async->nbyte);
    
    // use 16-bit interface on EVKB
    int len = async->nbyte / 2; // localize our loop endpoint for thread/ISR safety
    for(int i = 0; i < len; i++) {
      sdram[i] = src[i];
    }
    mcu_core_clean_data_cache();
    if (len % 2 > 0) {
      mcu_debug_log_error(MCU_DEBUG_SYS, "%s loc= %p nbyte= %d odd not handled\n", __func__, sdram, async->nbyte);
      //FIXME
    }

    if (async->nbyte >= 512) {
      mcu_debug_log_info (MCU_DEBUG_SYS, "%s [w0]= %0x [w255]= %0x", __func__,
          src[0], src[255]);
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
        if( o_flags & (DRIVE_FLAG_ERASE_BLOCKS | DRIVE_FLAG_ERASE_DEVICE | DRIVE_FLAG_INIT) ){

            uint16_t *s = (uint16_t *)config->start;
            int n = config->page_size * config->page_cnt;
            if( o_flags & DRIVE_FLAG_ERASE_BLOCKS ){
              s += attr->start;
              n = attr->end - attr->start;
            }

            // optimized 'memset' for 16-bit data interface
            int len = n / 2;
            for(int i = 0; i < len; i++) {
              s[i] = 0;
            }

            if (o_flags & DRIVE_FLAG_INIT ) {
              mcu_debug_log_info(MCU_DEBUG_SYS, "%s SETATTR INIT success", __func__);
            } else {
              mcu_debug_log_info(MCU_DEBUG_SYS, "%s SETATTR ERASE success", __func__);
            }
            return 0;
        }

        break;

    case I_DRIVE_ISBUSY:
        //mcu_debug_log_info(MCU_DEBUG_SYS, "%s ISBUSY", __func__);
        return 0;

    case I_DRIVE_GETINFO:
        info->o_flags = DRIVE_FLAG_ERASE_BLOCKS | DRIVE_FLAG_ERASE_DEVICE | DRIVE_FLAG_INIT;
        info->o_events = 0;
        info->addressable_size = 512;
        info->bitrate = SOS_BOARD_SYSTEM_CLOCK;
        info->erase_block_size = 4096;
        info->erase_block_time = 0;
        info->erase_device_time = 0;
        info->num_write_blocks = config->page_cnt * config->page_size / 4096;
        info->write_block_size = 4096;
        mcu_debug_log_info(MCU_DEBUG_SYS, "%s GETINFO", __func__);
        break;

    default:
        mcu_debug_log_error(MCU_DEBUG_SYS, "%s default EINVAL", __func__);
        return SYSFS_SET_RETURN(EINVAL);
    }
    return 0;
}

int ramdisk_close(const devfs_handle_t * handle){
    mcu_debug_log_info(MCU_DEBUG_SYS, "%s", __func__);
    return 0;
}

