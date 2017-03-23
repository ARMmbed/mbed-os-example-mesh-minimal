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
#include "socket_example.h"
#include "common_functions.h"
#include "ip6string.h"
#include "mbed-trace/mbed_trace.h"

DigitalOut command_led(LED1);
NetworkInterface * network_interface;

static void udp_main();
  
// mesh local multicast to all nodes
#define multicast_addr_str "ff03::1"
#define TRACE_GROUP "example"    
#define UDP_PORT 1234

uint8_t multi_cast_addr[16] = {0};  
static const char buffer_on[2] = {'o','n'};
static const char buffer_off[3] = {'o','f','f'};

InterruptIn my_button(SW2);
UDPSocket* my_socket;
// thread and queue for sending messages from button press
Thread thread;
Queue<uint32_t, 10> queue;

bool button_status = 1;

void start_socket_example(NetworkInterface * interface)
{
    network_interface = interface;
    const char *ip = network_interface->get_ip_address();
    tr_debug("IP address: %s\n", ip ? ip : "None");
    stoip6(multicast_addr_str, strlen(multicast_addr_str), multi_cast_addr);
    udp_main();
}

// As this comes from isr, we cannot use printing or network functions directly from here.
void my_button_isr() {
    queue.put((uint32_t*)1);// dummy number 
}

void send_message() {   
     
    button_status = !button_status;
    tr_debug("send msg %d", button_status);        
    
    SocketAddress send_sockAddr(multi_cast_addr, NSAPI_IPv6, UDP_PORT);  
    if (button_status) {
        command_led = 0;
        my_socket->sendto(send_sockAddr, buffer_on, 2);
    }
    else {
        command_led = 1;
        my_socket->sendto(send_sockAddr, buffer_off, 3);
    }
}

void queue_thread(void const *args) {
    while (1) {
        osEvent evt = queue.get();
        if (evt.status != osEventMessage) {
            tr_debug("queue->get() returned %02x status\n\r", evt.status);
        } else {            
            send_message();                
        }
    }
}

static void udp_main()
{    
    uint8_t buffer[5];    
    my_socket = new UDPSocket(network_interface);      
    my_socket->set_blocking(true);    
    my_socket->bind(UDP_PORT);
    
    // create own thread for queueu. Button call-back come from ISR.
    thread.start(callback(&queue, &queue_thread));
    my_button.fall(&my_button_isr);
    
    tr_debug("start listening on port %u !!!!!", UDP_PORT);
    while (true) {

        // Read data from the socket
        SocketAddress source_addr;
        memset(buffer, 0, sizeof(buffer));
        int length = my_socket->recvfrom(&source_addr, buffer, sizeof(buffer) - 1);
        if (length <= 0) {
            tr_debug("Error with socket recvfrom: %i\n", length);
            continue;
        }
        tr_debug("Packet from %s\n", source_addr.get_ip_address());
       
        // Handle command - "on", "off"
        if (strcmp((char*)buffer, "on") == 0) {
            tr_debug("Turning led on\n");
            command_led = 0;
        }
        if (strcmp((char*)buffer, "off") == 0) {
            tr_debug("Turning led off\n");
            command_led = 1;
        }      
    }
}