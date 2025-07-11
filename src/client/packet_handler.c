#include "packet_handler.h"

#include <stdio.h>

#include "ui.h"

static bool handle_greet(/*int sockfd, */ struct hoopoe_data_greet data)
{
    static char str[64];
    snprintf(str, 64, "server name: %s", data.name);
    hoopoe_ui_new_message(str);
    return true;
}

static bool handle_ping(int sockfd, struct hoopoe_data_ping data)
{
    // just send the same packet because what we need to send is just same.
    struct hoopoe_packet_data packet_data;
    packet_data.data_ping = data;
    return hoopoe_send_packet(sockfd, HOOPOE_PING, packet_data);
}

static bool handle_message(int sockfd, struct hoopoe_data_message data)
{
    hoopoe_ui_new_message(data.message);
    return true;
}

bool hoopoe_client_handle_packet(int sockfd, hoopoe_packet_type type,
                                 struct hoopoe_packet_data data)
{
    switch (type) {
    case HOOPOE_GREET:
        return handle_greet(/*sockfd, */ data.data_greet);
    case HOOPOE_PING:
        return handle_ping(sockfd, data.data_ping);
    case HOOPOE_MESSAGE:
        return handle_message(sockfd, data.data_message);
    default:
        printf("Invalid packet type %u\n", type);
        return false;
    }
}
