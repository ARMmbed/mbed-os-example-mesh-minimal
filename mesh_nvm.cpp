/*
 * Copyright (c) 2018 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"

#define MESH_LOWPAN     1
#define MESH_THREAD     2
/* At the moment, Thread builds using K64F support NVM */
#if MBED_CONF_APP_MESH_TYPE == MESH_THREAD && defined(TARGET_K64F)

#include "LittleFileSystem.h"
#include "SDBlockDevice.h"
#include "HeapBlockDevice.h"
#include "ns_file_system.h"
#include "mbed_trace.h"

#define TRACE_GROUP "mnvm"

/* By default use HEAP as NVM storage, comment macro in order to use SD card */
#define USE_HEAP_BLOCK_DEVICE

LittleFileSystem *fs;
BlockDevice *bd;

void mesh_nvm_initialize()
{
    fs = new LittleFileSystem("fs");
#ifdef USE_HEAP_BLOCK_DEVICE
    const char *bd_info = "NVM: Heap";
    bd = new HeapBlockDevice(16 * 512, 512);
#else
    const char *bd_info = "NVM: SD";
    bd = new SDBlockDevice(MBED_CONF_SD_SPI_MOSI, MBED_CONF_SD_SPI_MISO, MBED_CONF_SD_SPI_CLK, MBED_CONF_SD_SPI_CS);
#endif

     tr_debug("%s", bd_info);
    int mount_status = fs->mount(bd);
    if (mount_status) {
        tr_warning("mount error: %d, trying format...", mount_status);
        mount_status = fs->reformat(bd);
        tr_info("reformat %s (%d)", mount_status ? "failed" : "OK", mount_status);
    }

    if (!mount_status) {
        ns_file_system_set_root_path("/fs/");
    }
}

#else /* MBED_CONF_APP_MESH_TYPE == MESH_THREAD && defined(TARGET_K64F) */
void mesh_nvm_initialize()
{
    /* NVM not supported */
}
#endif  /* MBED_CONF_APP_MESH_TYPE == MESH_THREAD && defined(TARGET_K64F) */

