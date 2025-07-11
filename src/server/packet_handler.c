#include "packet_handler.h"

#include <stdio.h>
#include <string.h>

#include "../options.h"
#include "../shared/time.h"

static bool handle_greet(struct hoopoe_user *user,
                         struct hoopoe_data_greet data)
{
    // making sure that original name won't get mutated.
    static char *name = NULL;
    if (name == NULL)
        name = strdup(hoopoe_name());

    printf("Client '%s' greeted us!\n", data.name);
    user->name = strdup(data.name);

    // Greet back
    struct hoopoe_packet_data send_data;
    send_data.data_greet.name = name;
    return hoopoe_send_packet(user->fd, HOOPOE_GREET, send_data);
}

static bool handle_ping(struct hoopoe_user *user, struct hoopoe_data_ping data)
{
    uint64_t current_time = hoopoe_time_milliseconds();
    uint64_t sent_time = data.time;

    user->ping = current_time - sent_time;

    printf("Time diff with %s is %zu\n", user->name, user->ping);
    return true;
}

static bool handle_message(struct hoopoe_user *user,
                           struct hoopoe_data_message data)
{
    // TODO: sanitize input!
    static char formatted[200];
    snprintf(formatted, 200, "[%s] %s", user->name, data.message);

    struct hoopoe_packet_data send_data;
    send_data.data_message.message = formatted;

    uint32_t count;
    struct hoopoe_user **users = hoopoe_get_users(&count);

    for (uint32_t i = 0; i < count; i++) {
        hoopoe_send_packet(users[i]->fd, HOOPOE_MESSAGE, send_data);
    }

    return true;
}

bool hoopoe_server_handle_packet(struct hoopoe_user *user,
                                 hoopoe_packet_type type,
                                 struct hoopoe_packet_data data)
{
    switch (type) {
    case HOOPOE_GREET:
        return handle_greet(user, data.data_greet);
    case HOOPOE_PING:
        return handle_ping(user, data.data_ping);
    case HOOPOE_MESSAGE:
        return handle_message(user, data.data_message);
    default:
        printf("Invalid packet type %u\n", type);
        return false;
    }
}
