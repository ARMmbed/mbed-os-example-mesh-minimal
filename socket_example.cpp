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

DigitalOut command_led(LED1);
NetworkInterface * network_interface;
Thread udp_thread;
static void udp_main();
static void tcp_main(const void * sock);


void start_socket_example(NetworkInterface * interface)
{
    network_interface = interface;
    udp_thread.start(udp_main);
}

static void udp_main()
{
    TCPSocket * tcp_socket = NULL;
    Thread * tcp_thread = NULL;
    uint8_t buffer[32];

    // Setup UDP socket
    UDPSocket sock(network_interface);
    sock.set_blocking(true);
    sock.bind(1234);

    while (true) {

        // Read data from the socket
        SocketAddress source_addr;
        memset(buffer, 0, sizeof(buffer));
        int length = sock.recvfrom(&source_addr, buffer, sizeof(buffer) - 1);
        if (length <= 0) {
            printf("Error with socket recvfrom: %i\n", length);
            continue;
        }
        printf("Packet from %s\n", source_addr.get_ip_address());

        // Send message back
        uint8_t * data = (uint8_t *)"Packet recieved\n";
        int ret = sock.sendto(source_addr, data, strlen((char*)data));
        if (ret <= 0) {
            printf("Error with socket sendto: %i\n", ret);
            continue;
        }

        // Handle command - "on", "off", "close" or "connect"
        if (strcmp((char*)buffer, "on") == 0) {
            printf("Turning led on\n");
            command_led = 0;
        }
        if (strcmp((char*)buffer, "off") == 0) {
            printf("Turning led off\n");
            command_led = 1;
        }
        if (strcmp((char*)buffer, "close") == 0) {
            if (tcp_socket != NULL) {
                printf("Closing TCP connection\n");
                tcp_socket->close();
                tcp_socket = NULL;
            }
        }
        uint16_t port = 0;
        if (1 == sscanf((char*)buffer, "connect %hu", &port)) {

            // Close the TCP socket if one is open
            if (tcp_socket != NULL) {
                tcp_socket->close();
                tcp_thread->join();
                delete tcp_socket;
                delete tcp_thread;
            }

            // Setup address
            SocketAddress addr(source_addr);
            addr.set_port(port);

            // Create and connect socket
            tcp_socket = new TCPSocket(network_interface);
            ret = tcp_socket->connect(addr);

            // Send socket to the TCP reader thread
            if (0 == ret) {
                tcp_thread = new Thread();
                tcp_thread->start(callback(tcp_main, (void *)tcp_socket));
            } else {
                printf("Socket connect failed\n");
                delete tcp_socket;
                tcp_socket = NULL;
            }
        }
    }
}

static void tcp_main(const void * sock)
{
    uint8_t data[64];
    TCPSocket *socket = (TCPSocket *)sock;
    printf("Socket connected\n");

    while (true) {
        // Read TCP data
        memset(data, 0, sizeof(data));
        int ret = socket->recv(data, sizeof(data));
        if (ret <= 0) {
            printf("TCP recv returned %i\n", ret);
            break;
        }
        printf("Read data: %s\n", data);

        // Send response
        char resp[] = "Got device message";
        ret = socket->send(resp, sizeof(resp));
        if (ret <= 0) {
            printf("TCP send returned %i\n", ret);
            break;
        }

    }
}
