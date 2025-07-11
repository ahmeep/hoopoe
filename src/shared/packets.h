#ifndef HOOPOE_PACKETS_H_
#define HOOPOE_PACKETS_H_

#include <stdbool.h>
#include <stdint.h>

struct hoopoe_data_greet {
    char *name;
    // add more info?
};

struct hoopoe_data_ping {
    uint64_t time;
};

struct hoopoe_data_message {
    char *message;
};

struct hoopoe_packet_data {
    uint32_t data_size;
    uint8_t *data;

    /* Parsed data */
    union {
        struct hoopoe_data_greet data_greet;
        struct hoopoe_data_ping data_ping;
        struct hoopoe_data_message data_message;
    };
};

typedef enum {
    HOOPOE_GREET = 0,
    HOOPOE_PING,
    HOOPOE_MESSAGE,
} hoopoe_packet_type;

bool hoopoe_recv_packet(int sockfd, hoopoe_packet_type *type,
                        struct hoopoe_packet_data *data);
bool hoopoe_send_packet(int sockfd, hoopoe_packet_type type,
                        struct hoopoe_packet_data data);
void hoopoe_free_packet(hoopoe_packet_type type,
                        struct hoopoe_packet_data data);

#endif // HOOPOE_PACKETS_H_
