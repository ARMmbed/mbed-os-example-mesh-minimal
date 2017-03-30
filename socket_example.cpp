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
#include "nanostack/socket_api.h"
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
uint8_t receive_buffer[5];    

InterruptIn my_button(SW2);
UDPSocket* my_socket;
// queue for sending messages from button press.
EventQueue queue;
//EventQueue ev_queue;
int blink_id=-1;
int socket_id=-1;

volatile bool button_status = 1;

void start_socket_example(NetworkInterface * interface){
    network_interface = interface;
    const char *ip = network_interface->get_ip_address();
    tr_debug("IP address: %s\n", ip ? ip : "None");
    stoip6(multicast_addr_str, strlen(multicast_addr_str), multi_cast_addr);
    udp_main();
}

void cancel_blinking() {
    queue.cancel(blink_id);
    blink_id=-1;
}

void send_message() {
    if (blink_id>-1) {
        cancel_blinking();
    }
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

// As this comes from isr, we cannot use printing or network functions directly from here.
void my_button_isr() {
    queue.call(send_message);
}

void blink() {
    command_led = !command_led;
}

void receive() {
    // Read data from the socket
    SocketAddress source_addr;
    memset(receive_buffer, 0, sizeof(receive_buffer));
    bool something_in_socket=true;
    // read all messages
    while (something_in_socket) {
        int length = my_socket->recvfrom(&source_addr, receive_buffer, sizeof(receive_buffer) - 1);
        if (length > 0) {    
            tr_debug("Packet from %s\n", source_addr.get_ip_address());
             if (blink_id>-1) {
                cancel_blinking();
            }
            // Handle command - "on", "off"
            if (strcmp((char*)receive_buffer, "on") == 0) {
                tr_debug("Turning led on\n");
                command_led = 0;
                button_status=1;
            }
            if (strcmp((char*)receive_buffer, "off") == 0) {
                tr_debug("Turning led off\n");
                command_led = 1;
                button_status=0;
            }    
        }
        else if (length!=NSAPI_ERROR_WOULD_BLOCK) {
            tr_error("Error happened when receiving %d\n", length);        
            something_in_socket=false;
        }
        else {
            // there was nothing to read.    
            something_in_socket=false;
        }
    }    
}

void handle_socket() {
    // call-back might come from ISR
    queue.call(receive);
}

static void udp_main()
{
    my_socket = new UDPSocket(network_interface);
    my_socket->set_blocking(false);
    my_socket->bind(UDP_PORT);
    int16_t hops = 10;
    my_socket->setsockopt(SOCKET_IPPROTO_IPV6, SOCKET_IPV6_MULTICAST_HOPS, &hops, sizeof(hops));
    
    my_button.fall(&my_button_isr);
    //Start blinking to show that the Thread network is set-up
    blink_id = queue.call_every(1000, blink);
    //let's register the call-back function.
    //If something happens in socket (packets in or out), the call-back is called.
    my_socket->sigio(callback(handle_socket));
    // dispatch forever
    queue.dispatch();
}