#include "packets.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static bool recv_string(int sockfd, char **buf, uint32_t *size)
{
    uint32_t string_size;

    int ret = recv(sockfd, &string_size, sizeof(uint32_t), 0);

    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not recv");
        return false;
    }

    *buf = calloc(string_size + 1, sizeof(char));
    ret = recv(sockfd, *buf, string_size * sizeof(char), 0);

    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not recv");
        return false;
    }

    if (size != NULL)
        *size = string_size;

    return true;
}

static bool recv_greet(int sockfd, struct hoopoe_packet *ppacket)
{
    return recv_string(sockfd, &ppacket->data_greet.name, NULL);
}

static bool recv_ping(int sockfd, struct hoopoe_packet *ppacket)
{
    int ret = recv(sockfd, &ppacket->data_ping.time, sizeof(uint64_t), 0);
    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not recv");
        return false;
    }

    return true;
}

static bool recv_message(int sockfd, struct hoopoe_packet *ppacket)
{
    return recv_string(sockfd, &ppacket->data_message.message, NULL);
}

bool hoopoe_recv_packet(int sockfd, struct hoopoe_packet *ppacket)
{
    int ret = recv(sockfd, &ppacket->id, sizeof(uint8_t), 0);

    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not recv");
        return false;
    }

    switch (ppacket->id) {
    case HOOPOE_GREET:
        return recv_greet(sockfd, ppacket);
    case HOOPOE_PING:
        return recv_ping(sockfd, ppacket);
    case HOOPOE_MESSAGE:
        return recv_message(sockfd, ppacket);
    default:
        printf("Invalid packet id %u\n", ppacket->id);
        return false;
    }
}

static bool send_string(int sockfd, char *buf, uint32_t size)
{
    if (size == 0)
        size = strlen(buf);

    int ret = send(sockfd, &size, sizeof(uint32_t), 0);
    if (ret <= 0) {
        perror("could not send");
        return false;
    }

    ret = send(sockfd, buf, size * sizeof(char), 0);
    if (ret <= 0) {
        perror("could not send");
        return false;
    }

    return true;
}

static bool send_greet(int sockfd, struct hoopoe_packet ppacket)
{
    return send_string(sockfd, ppacket.data_greet.name, 0);
}

static bool send_ping(int sockfd, struct hoopoe_packet ppacket)
{
    int ret = send(sockfd, &ppacket.data_ping.time, sizeof(uint64_t), 0);
    if (ret <= 0) {
        perror("could not send");
        return false;
    }

    return true;
}

static bool send_message(int sockfd, struct hoopoe_packet ppacket)
{
    return send_string(sockfd, ppacket.data_message.message, 0);
}

bool hoopoe_send_packet(int sockfd, struct hoopoe_packet packet)
{
    int ret = send(sockfd, &packet.id, sizeof(uint8_t), 0);
    if (ret <= 0) {
        perror("could not send");
        return false;
    }

    switch (packet.id) {
    case HOOPOE_GREET:
        return send_greet(sockfd, packet);
    case HOOPOE_PING:
        return send_ping(sockfd, packet);
    case HOOPOE_MESSAGE:
        return send_message(sockfd, packet);
    default:
        printf("Invalid packet id %u\n", packet.id);
        return false;
    }

    return false;
}

void hoopoe_free_packet(struct hoopoe_packet packet)
{
    switch (packet.id) {
    case HOOPOE_GREET:
        if (packet.data_greet.name != NULL)
            free(packet.data_greet.name);
        break;
    case HOOPOE_MESSAGE:
        if (packet.data_message.message != NULL)
            free(packet.data_message.message);
        break;
    default:
        break;
    }
}
