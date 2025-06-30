#include "packet_handler.h"

#include <stdio.h>

#include "ui.h"

static bool handle_greet(/*int sockfd, */ struct hoopoe_packet packet)
{
    static char str[64];
    snprintf(str, 64, "server name: %s", packet.data_greet.name);
    hoopoe_ui_new_message(str);
    return true;
}

static bool handle_ping(int sockfd, struct hoopoe_packet packet)
{
    // just send the same packet because what we need to send is just same.
    return hoopoe_send_packet(sockfd, packet);
}

static bool handle_message(int sockfd, struct hoopoe_packet packet)
{
    hoopoe_ui_new_message(packet.data_message.message);
    return true;
}

bool hoopoe_client_handle_packet(int sockfd, struct hoopoe_packet packet)
{
    switch (packet.id) {
    case HOOPOE_GREET:
        return handle_greet(/*sockfd, */ packet);
    case HOOPOE_PING:
        return handle_ping(sockfd, packet);
    case HOOPOE_MESSAGE:
        return handle_message(sockfd, packet);
    default:
        printf("Invalid packet id %u\n", packet.id);
        return false;
    }
}
