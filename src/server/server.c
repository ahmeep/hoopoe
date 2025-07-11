#include "server.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared/connection_info.h"
#include "../shared/packets.h"
#include "../shared/time.h"
#include "packet_handler.h"
#include "pollfd_manager.h"

static char *hostname;
static uint16_t port;

static int sockfd;
static bool running = true;

static void accept_client_connection()
{
    int client = accept(sockfd, NULL, NULL);

    if (client > 0) {
        if (!hoopoe_add_pollfd(client, POLLIN | POLLOUT)
            || !hoopoe_add_user(client, NULL)) {
            // these should not fail unless there is no memory.
            running = false;
        }
    } else {
        perror("could not accept client connection");
    }
}

static void handle_client_connection(int fd, short revents)
{
    bool closed = false;

    struct hoopoe_user *user = hoopoe_get_user(fd);

    if (revents & POLLIN) {
        hoopoe_packet_type type;
        struct hoopoe_packet_data packet_data = {0};
        if (!hoopoe_recv_packet(fd, &type, &packet_data)
            || !hoopoe_server_handle_packet(user, type, packet_data))
            closed = true;
        hoopoe_free_packet(type, packet_data);
    } else if (revents & POLLOUT) {
        uint64_t current_time = hoopoe_time_milliseconds();
        if (current_time - user->last_pinged > 5000) {
            user->last_pinged = current_time;

            struct hoopoe_packet_data ping_data;
            ping_data.data_ping.time = current_time;
            if (!hoopoe_send_packet(fd, HOOPOE_PING, ping_data))
                closed = true;
        }
    } else if (revents != 0) {
        perror("error while polling");
        closed = true;
    }

    if (closed) {
        close(fd);
        hoopoe_remove_user(fd);
        hoopoe_remove_pollfd(fd);
    }
}

void hoopoe_server_start()
{
    if (!hoopoe_get_connection_info(&hostname, &port))
        return;

    struct addrinfo *addrinfo;
    if (!hoopoe_get_addrinfo(hostname, port, &addrinfo))
        return;

    sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype,
                    addrinfo->ai_protocol);

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));

    if (bind(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen) != 0) {
        perror("Cannot bind socket");
        return;
    }

    if (listen(sockfd, 32) != 0) {
        perror("Cannot listen socket");
        return;
    }

    struct pollfd *pollfds;
    uint32_t pollfds_size;

    if (!hoopoe_add_pollfd(sockfd, POLLIN))
        return;

    while (running) {
        hoopoe_get_pollfds(&pollfds, &pollfds_size);
        int ret = poll(pollfds, pollfds_size, -1);

        if (ret == -1)
            break;

        if (ret == 0)
            continue;

        for (uint32_t i = 0; i < pollfds_size; i++) {
            int fd = pollfds[i].fd;
            short revents = pollfds[i].revents;
            if (fd == sockfd) {
                if (revents & POLLIN) {
                    accept_client_connection();
                } else if (revents != 0) {
                    perror("error while polling server sock");
                    running = false;
                    break;
                }
            } else {
                handle_client_connection(fd, revents);
            }
        }
    }

    close(sockfd);

    freeaddrinfo(addrinfo);
}
