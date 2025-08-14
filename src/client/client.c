#include "client.h"

#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../options.h"
#include "../shared/connection_info.h"
#include "../shared/packets.h"
#include "packet_handler.h"
#include "ui.h"

static char *hostname;
static uint16_t port;

static int sockfd;
static bool running = true;

static struct hoopoe_ui_context *ui_context;

void hoopoe_client_start()
{
    if (!hoopoe_get_connection_info(&hostname, &port))
        return;

    struct addrinfo *addrinfo;
    if (!hoopoe_get_addrinfo(hostname, port, &addrinfo))
        return;

    hoopoe_ui_start(&ui_context);

    sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype,
                    addrinfo->ai_protocol);
    if (sockfd <= 0) {
        perror("Cannot create socket");
        freeaddrinfo(addrinfo);
        return;
    }

    if (connect(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen) != 0) {
        perror("Cannot connect socket");
        freeaddrinfo(addrinfo);
        return;
    }

    bool greeted = false;

    struct pollfd pollfd;
    pollfd.fd = sockfd;
    pollfd.events = POLLIN | POLLOUT;

    while (running) {
        int ret = poll(&pollfd, 1, -1);

        if (ret == -1)
            break;

        if (ret == 0)
            continue;

        if (pollfd.revents & POLLIN) {
            if (!hoopoe_recv_packets(sockfd, hoopoe_client_handle_packet))
                break;
        }

        if (pollfd.revents & POLLOUT) {
            if (ui_context->message_queue_size > 0) {
                pthread_mutex_lock(&ui_context->ui_mutex);

                char *message =
                    ui_context
                        ->message_queue[ui_context->message_queue_size - 1];

                struct hoopoe_packet_data packet_data;
                packet_data.data_message.message = message;

                if (!hoopoe_send_packet(sockfd, HOOPOE_MESSAGE, packet_data))
                    break;

                free(message);
                ui_context->message_queue_size--;

                pthread_mutex_unlock(&ui_context->ui_mutex);
            }

            if (!greeted) {
                char *temp_name_buf = strdup(hoopoe_name());

                struct hoopoe_packet_data packet_data;
                packet_data.data_greet.name = temp_name_buf;

                hoopoe_send_packet(sockfd, HOOPOE_GREET, packet_data);

                free(temp_name_buf);
                greeted = true;
            }
        }
    }

    close(sockfd);

    freeaddrinfo(addrinfo);

    hoopoe_ui_finish();
}
