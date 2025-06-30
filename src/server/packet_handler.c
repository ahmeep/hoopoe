#include "packet_handler.h"

#include <stdio.h>
#include <string.h>

#include "../options.h"
#include "../shared/time.h"

static bool handle_greet(struct hoopoe_user *user, struct hoopoe_packet packet)
{
    // making sure that original name won't get mutated.
    static char *name = NULL;
    if (name == NULL)
        name = strdup(hoopoe_name());

    printf("Client '%s' greeted us!\n", packet.data_greet.name);
    user->name = strdup(packet.data_greet.name);

    // Greet back
    struct hoopoe_packet packet_buf;
    packet_buf.id = HOOPOE_GREET;
    packet_buf.data_greet.name = name;
    return hoopoe_send_packet(user->fd, packet_buf);
}

static bool handle_ping(struct hoopoe_user *user, struct hoopoe_packet packet)
{
    uint64_t current_time = hoopoe_time_milliseconds();
    uint64_t sent_time = packet.data_ping.time;

    user->ping = current_time - sent_time;

    printf("Time diff with %s is %zu\n", user->name, user->ping);
    return true;
}

static bool handle_message(struct hoopoe_user *user,
                           struct hoopoe_packet packet)
{
    // TODO: sanitize input!
    static char formatted[200];
    snprintf(formatted, 200, "[%s] %s", user->name,
             packet.data_message.message);

    struct hoopoe_packet message_packet;
    message_packet.id = HOOPOE_MESSAGE;
    message_packet.data_message.message = formatted;

    uint32_t count;
    struct hoopoe_user **users = hoopoe_get_users(&count);

    for (uint32_t i = 0; i < count; i++) {
        hoopoe_send_packet(users[i]->fd, message_packet);
    }

    return true;
}

bool hoopoe_server_handle_packet(struct hoopoe_user *user,
                                 struct hoopoe_packet packet)
{
    switch (packet.id) {
    case HOOPOE_GREET:
        return handle_greet(user, packet);
    case HOOPOE_PING:
        return handle_ping(user, packet);
    case HOOPOE_MESSAGE:
        return handle_message(user, packet);
    default:
        printf("Invalid packet id %u\n", packet.id);
        return false;
    }
}
