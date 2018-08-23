/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
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
#include "rtos.h"
#include "NanostackInterface.h"
#include "mbed-trace/mbed_trace.h"
#include "mesh_nvm.h"

#if MBED_CONF_APP_ENABLE_LED_CONTROL_EXAMPLE
#include "mesh_led_control_example.h"
#endif

void trace_printer(const char* str) {
    printf("%s\n", str);
}

#define ATMEL   1
#define MCR20   2
#define NCS36510 3
#define KW24D   4

#define MESH_LOWPAN     3
#define MESH_THREAD     4

#if MBED_CONF_APP_RADIO_TYPE == ATMEL
#include "NanostackRfPhyAtmel.h"
NanostackRfPhyAtmel rf_phy(ATMEL_SPI_MOSI, ATMEL_SPI_MISO, ATMEL_SPI_SCLK, ATMEL_SPI_CS,
                           ATMEL_SPI_RST, ATMEL_SPI_SLP, ATMEL_SPI_IRQ, ATMEL_I2C_SDA, ATMEL_I2C_SCL);
#elif MBED_CONF_APP_RADIO_TYPE == MCR20 || MBED_CONF_APP_RADIO_TYPE == KW24D
#include "NanostackRfPhyMcr20a.h"
NanostackRfPhyMcr20a rf_phy(MCR20A_SPI_MOSI, MCR20A_SPI_MISO, MCR20A_SPI_SCLK, MCR20A_SPI_CS, MCR20A_SPI_RST, MCR20A_SPI_IRQ);

#elif MBED_CONF_APP_RADIO_TYPE == NCS36510
#include "NanostackRfPhyNcs36510.h"
NanostackRfPhyNcs36510 rf_phy;
#endif //MBED_CONF_APP_RADIO_TYPE

#if MBED_CONF_APP_MESH_TYPE == MESH_LOWPAN
LoWPANNDInterface mesh;
#elif MBED_CONF_APP_MESH_TYPE == MESH_THREAD
ThreadInterface mesh;
#endif //MBED_CONF_APP_MESH_TYPE

static Mutex SerialOutMutex;

void thread_eui64_trace()
{
#if MBED_CONF_APP_MESH_TYPE == MESH_THREAD
   uint8_t eui64[8] = {0};
   mesh.device_eui64_get(eui64);
   printf("Device EUI64 address = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7]);
#endif
}

void serial_out_mutex_wait()
{
    SerialOutMutex.lock();
}

void serial_out_mutex_release()
{
    SerialOutMutex.unlock();
}

int main()
{
    mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);
    mbed_trace_mutex_wait_function_set( serial_out_mutex_wait );
    mbed_trace_mutex_release_function_set( serial_out_mutex_release );
    
    printf("Start mesh-minimal application\n");
    printf("Build: %s %s\nMesh type: %d\n", __DATE__, __TIME__, MBED_CONF_APP_MESH_TYPE);
#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version: %d.%d.%d\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

#if MBED_CONF_APP_ENABLE_LED_CONTROL_EXAMPLE
    if (MBED_CONF_APP_BUTTON != NC && MBED_CONF_APP_LED != NC) {
        start_blinking();
    } else {
        printf("pins not configured. Skipping the LED control.\n");
    }
#endif
    mesh.initialize(&rf_phy);
    thread_eui64_trace();
    mesh_nvm_initialize();
    printf("Connecting...\n");
    int error = mesh.connect();
    if (error) {
        printf("Connection failed! %d\n", error);
        return error;
    }

    while (NULL == mesh.get_ip_address())
        Thread::wait(500);

    printf("Connected. IP = %s\n", mesh.get_ip_address());

#if MBED_CONF_APP_ENABLE_LED_CONTROL_EXAMPLE
    // Network found, start socket example
    if (MBED_CONF_APP_BUTTON != NC && MBED_CONF_APP_LED != NC) {
        cancel_blinking();
        start_mesh_led_control_example((NetworkInterface *)&mesh);
    }
#endif
}
