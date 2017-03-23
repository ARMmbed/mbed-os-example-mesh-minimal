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
#include "socket_example.h"

#define ATMEL   1
#define MCR20   2
#define NCS36510 3

#define MESH_LOWPAN     3
#define MESH_THREAD     4


#if MBED_CONF_APP_TRACE
#include "mbed-trace/mbed_trace.h"

void trace_printer(const char* str) {
    printf("%s\n", str);
}
#endif // MBED_CONF_APP_TRACE


#if MBED_CONF_APP_RADIO_TYPE == ATMEL
#include "NanostackRfPhyAtmel.h"
NanostackRfPhyAtmel rf_phy(ATMEL_SPI_MOSI, ATMEL_SPI_MISO, ATMEL_SPI_SCLK, ATMEL_SPI_CS,
                           ATMEL_SPI_RST, ATMEL_SPI_SLP, ATMEL_SPI_IRQ, ATMEL_I2C_SDA, ATMEL_I2C_SCL);
#elif MBED_CONF_APP_RADIO_TYPE == MCR20
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

int main()
{

#if MBED_CONF_APP_TRACE
	mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);
#endif
    printf("\n\nConnecting...\n");
    mesh.initialize(&rf_phy);

    if (mesh.connect()) {
        printf("Connection failed!\n");
        return -1;
    }

    while (NULL == mesh.get_ip_address())
        Thread::wait(500);

    printf("connected. IP = %s\n", mesh.get_ip_address());

    // Start socket example
    start_socket_example((NetworkInterface *)&mesh);        
}
