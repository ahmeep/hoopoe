#include "packets.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define MAX_IOVEC_SIZE 1024 * 1024

static bool decode_packet(hoopoe_packet_type type,
                          struct hoopoe_packet_data *data)
{
    switch (type) {
    case HOOPOE_GREET:
        if (data->data[data->data_size - 1] != 0)
            return false;

        data->data_greet.name = (char *)data->data;
        return true;
    case HOOPOE_PING:
        if (data->data_size < 2 * sizeof(uint64_t))
            return false;

        data->data_ping.time = *((uint64_t *)data->data);
        data->data_ping.old_ms = *((uint64_t *)data->data + 1);
        return true;
    case HOOPOE_MESSAGE:
        if (data->data[data->data_size - 1] != 0)
            return false;

        data->data_message.message = (char *)data->data;
        return true;
    default:
        printf("Invalid packet type %u\n", type);
        return false;
    }
}

bool hoopoe_recv_packets(int sockfd, hoopoe_packet_handler handler_cb)
{
    static struct iovec vec = {0};
    if (vec.iov_base == NULL)
        vec.iov_base = malloc(MAX_IOVEC_SIZE);
    vec.iov_len = MAX_IOVEC_SIZE;

    ssize_t ret = readv(sockfd, &vec, 1);

    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not readv");
        return false;
    }

    uint32_t cur = 0;

    while (cur < ret) {
        hoopoe_packet_type packet_type = *((uint8_t *)(vec.iov_base + cur));
        cur += 1;

        struct hoopoe_packet_data packet_data;

        packet_data.data_size = *((uint32_t *)(vec.iov_base + cur));
        cur += 4;

        packet_data.data = ((uint8_t *)(vec.iov_base + cur));
        cur += packet_data.data_size;

        if (!decode_packet(packet_type, &packet_data))
            return false;

        if (!handler_cb(sockfd, packet_type, packet_data))
            return false;
    }

    return true;
}

static uint32_t get_encoding_size(hoopoe_packet_type type,
                                  struct hoopoe_packet_data data)
{
    switch (type) {
    case HOOPOE_GREET:
        return strlen(data.data_greet.name) + 1;
    case HOOPOE_PING:
        return 2 * sizeof(uint64_t);
    case HOOPOE_MESSAGE:
        return strlen(data.data_message.message) + 1;
    default:
        printf("Invalid packet type %u\n", type);
        return 0;
    }
}

static bool encode_packet(struct iovec *vec, hoopoe_packet_type type,
                          struct hoopoe_packet_data data)
{
    uint32_t size = get_encoding_size(type, data);

    if (size == 0 || vec->iov_len + 5 + size > MAX_IOVEC_SIZE)
        return false;

    *((uint8_t *)(vec->iov_base + vec->iov_len)) = type;
    vec->iov_len += 1;

    *((uint32_t *)(vec->iov_base + vec->iov_len)) = size;
    vec->iov_len += 4;

    switch (type) {
    case HOOPOE_GREET:
        memcpy(vec->iov_base + vec->iov_len, data.data_greet.name, size);
        break;
    case HOOPOE_PING:
        memcpy(vec->iov_base + vec->iov_len, &data.data_ping.time, size);
        break;
    case HOOPOE_MESSAGE:
        memcpy(vec->iov_base + vec->iov_len, data.data_message.message, size);
        break;
    default:
        printf("Invalid packet type %u\n", type);
        return false;
    }

    vec->iov_len += size;

    return true;
}

bool hoopoe_send_packet(int sockfd, hoopoe_packet_type type,
                        struct hoopoe_packet_data data)
{
    static struct iovec vec = {0};
    if (vec.iov_base == NULL)
        vec.iov_base = malloc(MAX_IOVEC_SIZE);

    vec.iov_len = 0;
    if (!encode_packet(&vec, type, data))
        return false;

    int ret = writev(sockfd, &vec, 1);

    if (ret <= 0) {
        perror("could not writev");
        return false;
    }

    return true;
}
