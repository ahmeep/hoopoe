#include "packets.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define MAX_PACKET_SIZE 1024 * 1024

static bool decode_greet(struct hoopoe_packet_data *data)
{
    /* null termination check */
    if (data->data[data->data_size - 1] != 0)
        return false;

    data->data_greet.name = (char *)data->data;
    return true;
}

static bool decode_ping(struct hoopoe_packet_data *data)
{
    if (data->data_size < sizeof(uint64_t))
        return false;

    data->data_ping.time = *((uint64_t *)data->data);
    return true;
}

static bool decode_message(struct hoopoe_packet_data *data)
{
    /* null termination check */
    if (data->data[data->data_size - 1] != 0)
        return false;

    data->data_message.message = (char *)data->data;
    return true;
}

bool hoopoe_recv_packet(int sockfd, hoopoe_packet_type *type,
                        struct hoopoe_packet_data *data)
{
    static struct iovec vec = {0};
    if (vec.iov_base == NULL)
        vec.iov_base = calloc(MAX_PACKET_SIZE, sizeof(uint8_t));
    vec.iov_len = MAX_PACKET_SIZE * sizeof(uint8_t);

    int ret = readv(sockfd, &vec, 1);

    if (ret == 0)
        return false;

    if (ret < 0) {
        perror("could not readv");
        return false;
    }

    *type = *((uint8_t *)vec.iov_base);
    data->data_size = vec.iov_len - sizeof(uint8_t);
    data->data = malloc(data->data_size);
    memcpy(data->data, vec.iov_base + sizeof(uint8_t), data->data_size);

    switch (*type) {
    case HOOPOE_GREET:
        return decode_greet(data);
    case HOOPOE_PING:
        return decode_ping(data);
    case HOOPOE_MESSAGE:
        return decode_message(data);
    default:
        printf("Invalid packet type %u\n", *type);
        return false;
    }
}

static void encode_greet(struct iovec *vec, struct hoopoe_data_greet data)
{
    size_t len = strlen(data.name) + 1;
    if (vec->iov_len + len > MAX_PACKET_SIZE)
        return;

    memcpy(vec->iov_base + vec->iov_len, data.name, len);
    vec->iov_len += len;
}

static void encode_ping(struct iovec *vec, struct hoopoe_data_ping data)
{
    size_t len = sizeof(uint64_t);
    if (vec->iov_len + len > MAX_PACKET_SIZE)
        return;

    *((uint64_t *)(vec->iov_base + vec->iov_len)) = data.time;
    vec->iov_len += len;
}

static void encode_message(struct iovec *vec, struct hoopoe_data_message data)
{
    size_t len = strlen(data.message) + 1;
    if (vec->iov_len + len > MAX_PACKET_SIZE)
        return;

    memcpy(vec->iov_base + vec->iov_len, data.message, len);
    vec->iov_len += len;
}

bool hoopoe_send_packet(int sockfd, hoopoe_packet_type type,
                        struct hoopoe_packet_data data)
{
    static struct iovec vec = {0};
    if (vec.iov_base == NULL)
        vec.iov_base = calloc(MAX_PACKET_SIZE, sizeof(uint8_t));

    vec.iov_len = sizeof(uint8_t);
    *((uint8_t *)vec.iov_base) = type;

    switch (type) {
    case HOOPOE_GREET:
        encode_greet(&vec, data.data_greet);
        break;
    case HOOPOE_PING:
        encode_ping(&vec, data.data_ping);
        break;
    case HOOPOE_MESSAGE:
        encode_message(&vec, data.data_message);
        break;
    default:
        free(vec.iov_base);
        printf("Invalid packet type %u\n", type);
        return false;
    }

    int ret = writev(sockfd, &vec, 1);

    if (ret <= 0) {
        perror("could not send");
        return false;
    }

    return true;
}

void hoopoe_free_packet(hoopoe_packet_type type, struct hoopoe_packet_data data)
{
    free(data.data);
}
