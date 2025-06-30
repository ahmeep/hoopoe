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

typedef enum {
    HOOPOE_GREET = 0,
    HOOPOE_PING,
    HOOPOE_MESSAGE,
} hoopoe_packet_id;

struct hoopoe_packet {
    hoopoe_packet_id id;
    union {
        struct hoopoe_data_greet data_greet;
        struct hoopoe_data_ping data_ping;
        struct hoopoe_data_message data_message;
    };
};

bool hoopoe_recv_packet(int sockfd, struct hoopoe_packet *ppacket);
bool hoopoe_send_packet(int sockfd, struct hoopoe_packet packet);
void hoopoe_free_packet(struct hoopoe_packet packet);

#endif // HOOPOE_PACKETS_H_
