#ifndef HOOPOE_PACKETS_H_
#define HOOPOE_PACKETS_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HOOPOE_GREET = 0,
    HOOPOE_PING,
    HOOPOE_MESSAGE,
} hoopoe_packet_type;

struct hoopoe_data_greet {
    char *name;
    // add more info?
};

struct hoopoe_data_ping {
    uint64_t time;
    uint64_t old_ms;
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

typedef bool (*hoopoe_packet_handler)(int, hoopoe_packet_type,
                                      struct hoopoe_packet_data);

bool hoopoe_recv_packets(int sockfd, hoopoe_packet_handler handler_cb);
bool hoopoe_send_packet(int sockfd, hoopoe_packet_type type,
                         struct hoopoe_packet_data data);

#endif // HOOPOE_PACKETS_H_
